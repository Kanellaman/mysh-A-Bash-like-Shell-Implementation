#include "Interface.h"

int main(char *argc, char **argv)
{
  ptr hs = NULL;
  int opt;
  char *str = malloc(LINE_SIZE), **tokens = NULL;
  printf("in-mysh-now:> ");
  int pid, ff = 0, i;
  char *copy = NULL;
  while (fgets(str, LINE_SIZE, stdin) != NULL)
  {
    hs = append(hs, str);
    if (tokens != NULL)
    {
      for (int i = 0; i < 20; i++)
        if (tokens[i] != NULL)
        {
          free(tokens[i]);
          tokens[i] = NULL;
        }
      free(tokens);
    }
    copy = malloc(strlen(str) * sizeof(char) + 1);
    strcpy(copy, str);
    tokens = tokenize(copy);
    if (!strcmp(tokens[0], "exit"))
    {
      printf("You are exiting mysh!\n");
      break;
    }
    if (!strcmp(tokens[0], "history"))
    {
      print(hs);
      break;
    }
    if (!strcmp(tokens[0], "cd"))
    {
      if (cd(tokens) != 0)
        perror("cd");
      printf("in-mysh-now:> ");
      continue;
    }
    pid = fork();
    if (pid == 0)
    {
      int in = 0, out = 0;
      char *em[2] = {tokens[0], NULL}, *args[20];

      for (int i = 0; i < 20; i++)
      {
        if (tokens[i] == NULL)
          break;
        args[i] = malloc(strlen(tokens[i]) * sizeof(char) + 1);
        strcpy(args[i], tokens[i]);
      }
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
    {
      wait(NULL);
    }
    printf("in-mysh-now:> ");
  }
  for (int i = 0; i < 20; i++)
    if (tokens[i] != NULL)
      free(tokens[i]);
  free(tokens);
  free(str);
  del(hs);
  return 0;
}