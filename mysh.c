#include "Interface.h"

int main(char *argc, char **argv)
{
  ptr hs = malloc(sizeof(struct history));
  hs = init(hs);
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
        frees(copy, str, tokens, "chdir");
      continue;
    }
    // if ((i = redirection(tokens, ">")) != -1)
    // {
    //   char *outfile = tokens[i + 1];
    //   if (outfile == NULL)
    //     frees(copy, str, tokens, "Missing file for redirection");
    //   if ((fdout = open(outfile, O_RDONLY)) == -1)
    //     frees(copy, str, tokens, "open");
    //   if (dup2(fdout, STDOUT_FILENO) == -1)
    //     frees(copy, str, tokens, "dup2");
    //   close(fdout);
    // }
    pid = fork();
    if (pid == 0)
    {
      char *em[2] = {tokens[0], NULL}, **args;
      args = em;
      int fdin, fdout;
      if ((i = redirection(tokens, "<")) != -1)
      {
        char *infile = tokens[i + 1];
        if (infile == NULL)
          frees(copy, str, tokens, "Missing file for redirection");
        if ((fdin = open(infile, O_RDONLY)) == -1)
          frees(copy, str, tokens, "open");
        if (dup2(fdin, STDIN_FILENO) == -1)
          frees(copy, str, tokens, "dup2");
        close(fdin);
      }
      int error = execvp(tokens[0], args);
      if (error == -1)
      {
        printf("Check if '%s' is a valid command\n", tokens[0]);
        frees(copy, str, tokens, "Execvp");
      }
    }
    else
      wait(NULL);
    printf("in-mysh-now:> ");
    free(tokens);
    free(copy);
  }
  free(str);
  free(hs);
  return 0;
}