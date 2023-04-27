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
    if (tok != NULL)
    {
      for (int j = 0; j < total; j++)
      {
        if (tok[j] != NULL)
        {
          for (int i = 0; i < TOKEN_NUM; i++)
            if (tok[j][i] != NULL)
            {
              free(tok[j][i]);
              tok[j][i] = NULL;
            }
            else
              break;
          free(tok[j]);
          tok[j] = NULL;
        }
      }
      free(tok);
      tok = NULL;
    }
    total = 0;
    hs = append(hs, str);

    tokens = tokenize(str, &globbuf);

    if (tokens == NULL)
    {
      free(tokens);
      printf("in-mysh-now:> ");
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
          if (tokens[i + 1] == NULL)
            break;
          if (!flag && !strcmp(tokens[i + 1], ";"))
          {
            i++;
            free(tokens[i]);
            if (tokens[i + 1] == NULL)
              break;
          }
          x = 0;
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
    else
      free(tokens);

    for (int j = 0; j < total; j++)
    {
      while ((num = hs_al(tok[j], &hs, &al, &str)) >= 0)
      {
        for (int i = 0; i < TOKEN_NUM; i++)
          if (tok[j][i] != NULL)
          {
            free(tok[j][i]);
            tok[j][i] = NULL;
          }
          else
            break;
        free(tok[j]);
        tok[j] = NULL;
        if (num > 0 && total == 1)
          hs = append(hs, str);
        tok[j] = tokenize(str, &globbuf);
      }
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
  if (tok != NULL)
  {
    for (int j = 0; j < total; j++)
    {
      if (tok[j] != NULL)
      {
        for (int i = 0; i < TOKEN_NUM; i++)
          if (tok[j][i] != NULL)
          {
            free(tok[j][i]);
            tok[j][i] = NULL;
          }
          else
            break;
        free(tok[j]);
      }
    }
    free(tok);
  }
  free(str);
  del(hs);
  dele(al);
  return 0;
}