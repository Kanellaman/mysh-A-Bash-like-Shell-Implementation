#include "Interface.h"

int main(char *argc, char **argv)
{
  ptr hs = NULL;
  alr al = NULL, alias;
  int opt;
  char *str = malloc(LINE_SIZE), **tokens = NULL;
  printf("in-mysh-now:> ");
  int pid, ff = 0, i = 2, num;
  char *copy = NULL;
  glob_t globbuf;
  int flags = GLOB_NOCHECK | GLOB_TILDE;
  while (i != 0)
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
    if (i == 1)
    {
      strcpy(str, get_command(hs, num));
      printf("Running command %s", str);
      i = 2;
    }
    else if (fgets(str, LINE_SIZE, stdin) == NULL)
      break;
    char *tmp = malloc(strlen(str) * sizeof(char) + 1);
    strcpy(tmp, str);
    tmp[strlen(tmp) - 1] = '\0';
    if ((alias = search(al, tmp)) != NULL)
      strcpy(str, alias->cmd);
    free(tmp);
    copy = malloc(strlen(str) * sizeof(char) + 1);
    strcpy(copy, str);
    tokens = tokenize(copy);
    if (tokens == NULL)
    {
      printf("in-mysh-now:> ");
      continue;
    }
    int j = 0;
    char **args = malloc(TOKEN_NUM * sizeof(char *));
    for (int i = 0; i < TOKEN_NUM; i++)
      args[i] = NULL;
    args[j++] = tokens[0];
    for (int i = 1; tokens[i] != NULL; i++)
    {
      if (strchr(tokens[i], '*') != NULL || strchr(tokens[i], '?') != NULL)
      {

        if (glob(tokens[i], flags, NULL, &globbuf) == 0)
        {
          for (size_t i = 0; i < globbuf.gl_pathc; i++)
          {
            args[j] = malloc(strlen(globbuf.gl_pathv[i]) * sizeof(char) + 1);
            strcpy(args[j++], globbuf.gl_pathv[i]);
          }
          globfree(&globbuf);
          free(tokens[i]);
        }
      }
      else
        args[j++] = tokens[i];
    }
    // for (int i = 0; i < TOKEN_NUM; i++)
    //   if (args[i] != NULL)
    //     printf("%s\n", args[i]);
    free(tokens);
    tokens = args;
    int lole = hs_al(tokens, &hs, &al);
    if (!strcmp(tokens[0], "exit"))
    {
      printf("You are exiting mysh!\n");
      i = 0;
    }
    else if (lole != 0)
    {
      if (lole == 1)
        i = lole;
    }
    else if (!strcmp(tokens[0], "cd"))
    {
      if (cd(tokens) != 0)
        perror("cd");
    }
    else
    {
      int i = 0, pipes = 0;
      while (tokens[i] != NULL)
      {
        if (!strcmp(tokens[i++], "|"))
          pipes++;
      }
      if (!strcmp(tokens[i - 1], "|"))
      {
        printf("Expected command after last pipe |\n");
        exit(0);
      }
      if (pipes != 0)
      {
        printf("%d", pipes);
        exit(0);
      }
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
        // for (int i = 0; i < TOKEN_NUM; i++)
        // {
        //   if (args[i] != NULL)
        //   printf("aa%s\n", args[i]);
        // }

        int error = execvp(args[0], args);
        if (error == -1)
        {
          perror("Execvp");
          printf("Check if '%s' is a valid command or alias\n", tokens[0]);
        }
        exit(0); // In case execvp returns with an error
      }
      else
      {
        wait(NULL);
      }
    }
    if (i != 1)
      hs = append(hs, str);
    if (i != 0 && i != 1)
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