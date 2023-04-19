#include "Interface.h"

int main(char *argc, char **argv)
{
  ptr hs = NULL;
  int opt;
  char *str = malloc(LINE_SIZE), **tokens = NULL;
  printf("in-mysh-now:> ");
  int pid, ff = 0, i = 2, num;
  char *copy = NULL;
  while (i != 0)
  {
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
    if (i == 1)
    {
      strcpy(str, get_command(hs, num));
      printf("Running command %s", str);
      i = 2;
    }
    else if (fgets(str, LINE_SIZE, stdin) == NULL)
      break;
    copy = malloc(strlen(str) * sizeof(char) + 1);
    strcpy(copy, str);
    tokens = tokenize(copy);
    // for (int i = 0; i < 20; i++)
    // {
    //   if (tokens[i] != NULL)
    //   {
    //     printf("789%s ", tokens[i]);
    //   }
    // }
    // continue;
    if (tokens == NULL)
    {
      printf("in-mysh-now:> ");
      continue;
    }
    else if (!strcmp(tokens[0], "exit"))
    {
      printf("You are exiting mysh!\n");
      i = 0;
    }
    else if (!strcmp(tokens[0], "history"))
    {
      if (tokens[1] == NULL)
      {
        print(hs);
        printf("in-mysh-now:> ");
        continue;
      }
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
        exit(0); // In case execvp returns with an error
      }
      else
      {
        wait(NULL);
      }
    }
    hs = append(hs, str);
    if (i != 0)
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