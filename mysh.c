#include "Interface.h"

int main(char *argc, char **argv)
{
  ptr hs = NULL;
  alr al = NULL;
  int opt;
  char *str = malloc(LINE_SIZE), **tokens = NULL, ***tok = NULL;
  printf("in-mysh-now:> ");
  int pid, ff = 0, i = 1, num = -5, total = 0;
  char *copy = NULL;
  glob_t globbuf;
  int flags = GLOB_NOCHECK | GLOB_TILDE;
  while (i != 0 && fgets(str, LINE_SIZE, stdin) != NULL)
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
      tokens = NULL;
      free(tok);
      tok = NULL;
    }
    if (tok != NULL)
    {
      for (int i = 0; total != 1 && i < total; i++)
        free(tok[i]);
      free(tok);
    }
    total = 0;
    hs = append(hs, str);

    tokens = tokenize(str, al, &globbuf);

    if (tokens == NULL)
    {
      printf("sdfin-mysh-now:> ");
      continue;
    }
    int p = 0, pipes = 0, pun = 0, amper = 0;
    while (tokens[p] != NULL)
    {
      if (!strcmp(tokens[p], "|"))
        pipes++;
      else if (!strcmp(tokens[p], ";"))
        pun++;
      else if (!strcmp(tokens[p], "&"))
      {
        if (tokens[p + 1] != NULL && !strcmp(tokens[p + 1], ";"))
          pun--;
        amper++;
      }
      p++;
    }

    total = pipes + pun + amper;
    if (!strcmp(tokens[p - 1], "|"))
    {
      printf("Expected command after last token\n");
      printf("in-mysh-now:> ");
      continue;
    }
    if (total > 0)
    {
      tok = malloc(sizeof(char **));
      int j = 0, x = 0;
      tok[0] = malloc(sizeof(char *));
      for (int i = 0; i < TOKEN_NUM && tokens[i] != NULL; i++)
      {
        int flag = 1;
        if (!strcmp(tokens[i], "|") || !strcmp(tokens[i], ";") || !(flag = strcmp(tokens[i], "&")))
        {
          tok[j][x] = tokens[i];
          tok[j] = realloc(tok[j], (2 + x) * sizeof(char **));
          tok[j][++x] = NULL;
          if (!flag && tokens[i + 1] != NULL && !strcmp(tokens[i + 1], ";"))
            i++;
          x = 0;
          if (tokens[i + 1] == NULL)
            break;
          j++;
          tok = realloc(tok, (j + 1) * sizeof(char **));
          tok[j] = malloc(sizeof(char *));
          continue;
        }
        tok[j][x] = tokens[i];
        tok[j] = realloc(tok[j], (2 + x) * sizeof(char **));
        tok[j][++x] = NULL;
      }
      total = j + 1;
    }
    if (tok == NULL)
    {
      total = 1;
      tok = malloc(sizeof(char **));
      tok[0] = tokens;
    }

    for (int j = 0; j < total; j++)
    {
      while ((num = hs_al(tok[j], &hs, &al, &str)) >= 0)
      {
        for (int i = 0; i < TOKEN_NUM; i++)
          if (tokens[i] != NULL)
          {
            free(tokens[i]);
            tokens[i] = NULL;
          }
        free(tokens);
        tokens = NULL;
        hs = append(hs, str);
        tokens = tokenize(str, al, &globbuf);
        tok[j] = tokens;
      }
      printf("%s", tok[j][0]);
      if (!strcmp(tok[j][0], "exit"))
      {
        printf("You are exiting mysh!\n");
        i = 0;
        break;
      }
      else if (!strcmp(tok[j][0], "cd"))
      {
        if (cd(tok[j]) != 0)
          perror("cd");
        continue;
      }
      if (num != -1)
      {
        pid = fork();
        if (pid == 0)
        {
          char **args = malloc(TOKEN_NUM * sizeof(char *));

          for (int i = 0; i < TOKEN_NUM; i++)
            args[i] = NULL;
          if (redirection(tok[j]) == -1)
            return -1;
          int i = 0, x = 0;
          while (tok[j][i] != NULL)
          {
            if (!strcmp(tok[j][i], "<") || !strcmp(tok[j][i], ">") || !strcmp(tok[j][i], ">>"))
            {
              i += 2;
              continue;
            }
            if (!strcmp(tok[j][i], ";") || !strcmp(tok[j][i], "|") || !strcmp(tok[j][i], "&"))
            {
              i++;
              continue;
            }
            if (tok[j][i][0] == '\"')
            {
              args[x] = malloc(strlen(tok[j][i]) * sizeof(char) - 1);
              strncpy(args[x], tok[j][i] + 1, strlen(tok[j][i]) - 2);
              args[x++][strlen(tok[j][i++]) - 1] = '\0';
              continue;
            }
            args[x] = malloc(strlen(tok[j][i]) * sizeof(char) + 1);
            strcpy(args[x++], tok[j][i++]);
          }

          int error = execvp(args[0], args);
          if (error == -1)
          {
            perror("Execvp");
            printf("Check if '%s' is a valid command or alias\n", tok[j][0]);
          }
          exit(0); // In case execvp returns with an error
        }
        else
          wait(NULL);
      }
    }
    if (i != 0)
      printf("in-mysh-now:> ");
  }
  for (int i = 0; i < TOKEN_NUM; i++)
    if (tokens[i] != NULL)
      free(tokens[i]);
  free(tokens);
  if (tok != NULL)
  {
    for (int i = 0; total != 1 && i < total; i++)
      free(tok[i]);
    free(tok);
  }
  free(str);
  del(hs);
  dele(al);
  return 0;
}