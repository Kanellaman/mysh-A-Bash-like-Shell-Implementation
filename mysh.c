#include "Interface.h"

int main(char *argc, char **argv)
{
  ptr hs = NULL;
  alr al = NULL;
  int opt;
  char *str = malloc(LINE_SIZE), **tokens = NULL, ***tok = NULL;
  printf("in-mysh-now:> ");
  int pid, ff = 0, i = 2, num;
  char *copy = NULL;
  glob_t globbuf;
  int flags = GLOB_NOCHECK | GLOB_TILDE;
  while (fgets(str, LINE_SIZE, stdin) != NULL)
  {
    if (tokens != NULL)
    {
      for (int i = 0; i < TOKEN_NUM; i++)
        if (tokens[i] != NULL)
        {
          free(tokens[i]);
          tokens[i] = NULL;
        }
      free(tokens);
    }
    hs = append(hs, str);

    tokens = tokenize(str, al, &globbuf);
    if (tokens == NULL)
    {
      printf("in-mysh-now:> ");
      continue;
    }

    while ((num = hs_al(tokens, &hs, &al)) > 0)
    {
      strcpy(str, get_command(hs, num));
      printf("%s", str);
      for (int i = 0; i < TOKEN_NUM; i++)
        if (tokens[i] != NULL)
        {
          free(tokens[i]);
          tokens[i] = NULL;
        }
      free(tokens);
      hs = append(hs, str);
      tokens = tokenize(str, al, &globbuf);
    }

    if (!strcmp(tokens[0], "exit"))
    {
      printf("You are exiting mysh!\n");
      break;
    }
    else if (!strcmp(tokens[0], "cd"))
    {
      if (cd(tokens) != 0)
        perror("cd");
      printf("in-mysh-now:> ");
      continue;
    }
    if (num != 0)
    {
      pid = fork();
      if (pid == 0)
      {
        char **args = malloc(TOKEN_NUM * sizeof(char *));

        for (int i = 0; i < TOKEN_NUM; i++)
          args[i] = NULL;
        if (redirection(tokens) == -1)
          return 0;
        int i = 0, j = 0;
        while (tokens[i] != NULL)
        {
          if (!strcmp(tokens[i], "<") || !strcmp(tokens[i], ">"))
          {
            i += 2;
            continue;
          }
          args[j] = malloc(strlen(tokens[i]) * sizeof(char) + 1);
          strcpy(args[j++], tokens[i++]);
        }

        int error = execvp(args[0], args);
        if (error == -1)
        {
          perror("Execvp");
          printf("Check if '%s' is a valid command or alias\n", tokens[0]);
        }
        exit(0); // In case execvp returns with an error
      }
      else
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
  dele(al);
  return 0;
}