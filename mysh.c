#include "Interface.h"

int main(char *argc, char **argv)
{
  ptr hs;
  init(hs);
  int opt;
  char *str = malloc(LINE_SIZE), **tokens;
  printf("in-mysh-now:> ");
  int i = 0, pid;
  while (fgets(str, LINE_SIZE, stdin) != NULL)
  {
    char *copy = malloc(strlen(str) * sizeof(char) + 1);
    strcpy(copy, str);
    tokens = tokenize(copy);
    if (!strcmp(tokens[0], "exit"))
    {

      free(tokens);
      free(copy);
      printf("You are exiting mysh!\n");
      break;
    }

    if (!strcmp(tokens[0], "cd"))
    {
      if (cd(tokens) != 0)
      {
        frees(copy, str, tokens, "chdir");
      }
    }
    else
    {
      int fdin = -1, fd = dup(STDIN_FILENO);
      if ((i = redirection(tokens)) != -1)
      {
        char *infile = tokens[i + 1];
        if (infile == NULL)
        {
          printf("Missing file for redirection\n");
          return 1;
        }
        fdin = open(infile, O_RDONLY);
        if (fdin == -1)
        {
          frees(copy, str, tokens, "open");
        }
        if (dup2(fdin, STDIN_FILENO) == -1)
        {
          frees(copy, str, tokens, "dup2");
        }
        close(fdin);
      }
      pid = fork();
      if (pid == 0)
      {
        char *em[2] = {tokens[0], NULL}, **args;
        args = em;
        int error = execvp(tokens[0], args);
        if (error == -1)
        {
          printf("Check if '%s' is a valid command\n", tokens[0]);
          frees(copy, str, tokens, "Execvp");
        }
      }
      else
      {
        wait(NULL);
        if (fdin != -1)
        {
          if (dup2(fd, STDIN_FILENO) == -1)
          {
            frees(copy, str, tokens, "dup2");
          }
        }
      }
    }
    printf("in-mysh-now:> ");
    free(tokens);
    free(copy);
  }
  i = 0;

  free(str);
  // free(hs);
  return 0;
}