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
    if (!strcmp(tokens[0], "exit"))
    {
      printf("You are exiting mysh!\n");
      i = 0;
    }
    else if (!strcmp(tokens[0], "history"))
    {
      if (tokens[1] == NULL)
        print(hs);
      else if (sscanf(tokens[1], "%d", &num) == 1 && tokens[2] == NULL)
        if (num > 0 && num <= hs->count)
        {
          printf("Getting the %dth command of the history...\n", num);
          i = 1;
        }
        else
          printf("There is no %dth command in history...Try command \"history\" to see the command history\n", num);
      else
        printf("Bad syntax of command \"history\".\nTry \"history\" or \"history <num>\" where num is in number in the range of 1..20\n");
    }
    else if (!strcmp(tokens[0], "createalias"))
    {
      if (tokens[3] != NULL && tokens[2][0] == '\"')
      {
        alr tmp;
        char *cmd = malloc((LINE_SIZE / 2));
        strncpy(cmd, tokens[2] + 1, strlen(tokens[2]) - 2);
        cmd[strlen(tokens[2]) - 1] = '\0';
        tmp = in(al, tokens[1], cmd);
        if (tmp != NULL)
          al = tmp;
        free(cmd);
      }
      else
        printf("Bad syntax of command \"createalias\".\nTry createalias <alias> \"command to alias\";\n");
    }
    else if (!strcmp(tokens[0], "destroyalias"))
    {
      if (tokens[2] != NULL)
      {
        al = delal(al, tokens[1]);
      }
      else
        printf("Bad syntax of command \"destroyalias\".\nTry destroyalias <alias>;\n");
    }
    else if (!strcmp(tokens[0], "cd"))
    {
      if (cd(tokens) != 0)
        perror("cd");
    }
    else
    {
      pid = fork();
      if (pid == 0)
      {
        int in = 0, out = 0;
        char *em[2] = {tokens[0], NULL}, *args[20];

        for (int i = 0; i < TOKEN_NUM; i++)
        {
          if (tokens[i] == NULL)
          {
            args[i] = NULL;
            break;
          }
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
  // globfree(&globbuf);
  return 0;
}