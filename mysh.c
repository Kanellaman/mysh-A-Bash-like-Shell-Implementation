#include "Interface.h"

int main(char *argc, char **argv)
{
  ptr hs = malloc(sizeof(struct history));
  hs = init(hs);
  int opt;
  char *str = malloc(LINE_SIZE), **tokens;
  printf("in-mysh-now:> ");
  int pid;
  char *copy = NULL;
  while (fgets(str, LINE_SIZE, stdin) != NULL)
  {
    if (copy != NULL)
    {
      free(tokens);
      free(copy);
    }
    copy = malloc(strlen(str) * sizeof(char) + 1);
    strcpy(copy, str);
    tokens = malloc(20 * sizeof(char *));
    tokenize(copy, &tokens);
    for (int i = 0; i < 20; i++)
    {
      printf("%s ", tokens[i]);
    }
    break;
    if (!strcmp(tokens[0], "exit"))
    {
      printf("You are exiting mysh!\n");
      break;
    }
    if (!strcmp(tokens[0], "cd"))
    {
      if (cd(tokens) != 0)
        frees(copy, str, tokens, "chdir");
    }
    else
    {
      pid = fork();
      if (pid == 0)
      {
        int in, out;
        char *em[2] = {tokens[0], NULL}, *args[20];

        for (int i = 0; i < 20; i++)
          args[i] = tokens[i];

        if (redirection(tokens, "<", copy, str) == -1 || redirection(tokens, ">", copy, str) == -1 || redirection(tokens, ">>", copy, str) == -1)
          return 0;

        if ((out = find(tokens, ">")) != -1 || (out = find(tokens, ">>")) != -1)
          args[out] = NULL;
        if ((in = find(tokens, "<")) != -1)
          args[in] = NULL;

        int error = execvp(args[0], args);
        if (error == -1)
        {
          perror("Execvp");
          printf("Check if '%s' is a valid command\n", tokens[0]);
        }
      }
      else
        wait(NULL);
    }
    printf("in-mysh-now:> ");
  }
  free(tokens);
  free(copy);
  free(str);
  free(hs);
  return 0;
}