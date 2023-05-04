#include "Interface.h"

pid_t bgpg = 0, fg = 0;
void sig_handler(int sig)
{
  if (sig == SIGINT)
    if (fg && killpg(-fg, sig) == -1)
    { // Send the signal to the job's process group
      perror("killpg");
      exit(EXIT_FAILURE);
    }

  if (sig == SIGTSTP)
  {
    if (fg && killpg(-fg, sig) == -1)
    { // Send the signal to the job's process group
      perror("killpg");
      exit(EXIT_FAILURE);
    }
    fg = 0;
  }
}
int main(char *argc, char **argv)
{
  char *str = malloc(LINE_SIZE), **tokens = NULL, ***tok = NULL;
  int pid, i = 1, num = -5, total = 0, status;
  ptr hs = NULL;
  alr al = NULL;
  signal(SIGINT, sig_handler);
  signal(SIGTSTP, sig_handler);
  signal(SIGTTOU, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  setpgid(0, getpid());
  tcsetpgrp(0, getpid());
  printf("in-mysh-now:> ");
  while (i != 0 && fgets(str, LINE_SIZE, stdin) != NULL)
  {
    waitpid(-1, &status, WNOHANG);

    tok = frees(tok, total);
    total = 0;
    tokens = tokenize(str);

    if (tokens == NULL)
    {
      free(tokens);
      printf("in-mysh-now:> ");
      continue;
    }
    hs = append(hs, str);
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
    int output = -1, input = 0, fds[2];
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
        tok[j] = tokenize(str);
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
      if (num == -1)
        continue;
      int last = -1, status;
      pid_t pid, pgid;
      while (tok[j][++last] != NULL)
        ;
      if (!strcmp(tok[j][last - 1], "|"))
      {
        pipe(fds);
        output = fds[1];
        printf("%d", output);
        p = 1;
      }
      else if (output > -1)
      {
        output = 1;
        p = 0;
      }
      pid = fork();
      if (pid == 0)
      {
        if (input)
        {
          dup2(input, 0);
          close(input);
        }
        if (output != 1)
        {
          dup2(output, 1);
          close(output);
          close(fds[0]);
        }
        if (!strcmp(tok[j][last - 1], "&"))
          setpgid(pid, bgpg);
        else
        {
          if (!fg)
            fg = pid;
          setpgid(getpid(), fg);
          tcsetpgrp(0, fg);
        }

        if (redirection(tok[j]) == -1)
          return 0;

        char **args = cleanup(tok[j]);
        int error = execvp(args[0], args);
        if (error == -1)
        {
          perror("Execvp");
          printf("Check if '%s' is a valid command or alias\n", tok[j][0]);
        }
        return 0; // In case execvp returns with an error
      }
      else
      {
        if (output != 1)
          close(output);
        if (input != 0)
          close(input);
        input = fds[0];
        if (strcmp(tok[j][last - 1], "&"))
        {
          if (!fg)
            fg = pid;
          setpgid(pid, fg);
          tcsetpgrp(0, fg);
          if (strcmp(tok[j][last - 1], "|"))
          {
            pid_t p = 0;
            while ((waitpid(-fg, &status, WUNTRACED) != -1 || errno != ECHILD) && errno == EINTR)
              ;
            fg = 0;
          }
          tcsetpgrp(0, getpid());
        }
        else
        {
          if (!bgpg)
            bgpg = pid;
          setpgid(pid, bgpg);
        }
      }
    }
    if (i != 0)
      printf("in-mysh-now:> ");
  }
  tok = frees(tok, total);
  free(str);
  del(hs);
  dele(al);
  return 0;
}