#include "Interface.h"

pid_t fg = 0;
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
  struct sigaction sa;
  sa.sa_handler = sig_handler;
  sa.sa_flags = SA_SIGINFO;
  int k = 5;
  char *str = malloc(LINE_SIZE), **tokens = NULL, ***tok = NULL;
  pid_t bgpg = 0;
  int pid, i = 1, num = -5, total = 0, status;
  ptr hs = NULL;
  alr al = NULL;
  if (sigaction(SIGINT, &sa, NULL) == -1)
  {
    perror("sigaction for SIGINT");
    exit(EXIT_FAILURE);
  }

  if (sigaction(SIGTSTP, &sa, NULL) == -1)
  {
    perror("sigaction for SIGTSTP");
    exit(EXIT_FAILURE);
  }
  signal(SIGTTOU, SIG_IGN);
  signal(SIGTTIN, SIG_IGN);
  setpgid(0, getpid());
  tcsetpgrp(0, getpid());
  printf("in-mysh-now:> ");
  while (i != 0 && fgets(str, LINE_SIZE, stdin) != NULL)
  {
    int child = waitpid(-1, &status, WNOHANG);
    if (child > 0)
      printf("\n            Process with pid %d terminated\n\n", child);
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
    // int p = 0, pipes = 0, pun = 0, amper = 0;
    // while (tokens[p] != NULL)
    // {
    //   if (!strcmp(tokens[p], "|"))
    //     pipes++;
    //   else if (!strcmp(tokens[p], ";"))
    //     pun++;
    //   else if (!strcmp(tokens[p], "&"))
    //   {
    //     if (tokens[p + 1] != NULL && !strcmp(tokens[p + 1], ";"))
    //       pun--;
    //     amper++;
    //   }
    //   p++;
    // }
    // total = pipes + pun + amper;
    // if (!strcmp(tokens[p - 1], "|"))
    // {
    //   printf("Expected command after last token\n");
    //   printf("in-mysh-now:> ");
    //   continue;
    // }
    // if (total > 0)
    // {
    //   tok = separate(tok,tokens, &total);
    // }
    tok = separate(tok, tokens, &total);
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
      }
      else if (output > -1)
        output = 1;
      pid = fork();
      if (pid == 0)
      {
        if (input)
        {
          dup2(input, 0);
          close(input);
        }
        if (output != 1 && output != -1)
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
          tcsetpgrp(0, fg); // Reserve the terminal for input-output for you
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
        if (output != 1 && output != -1)
          close(output);
        if (input != 0)
          close(input);
        if (!strcmp(tok[j][last - 1], "|"))
          input = fds[0];
        if (strcmp(tok[j][last - 1], "&"))
        {
          if (!fg)
            fg = pid;
          setpgid(pid, fg);
          tcsetpgrp(0, fg);
          if (strcmp(tok[j][last - 1], "|"))
          {
            while (1)
            {
              pid_t pid = waitpid(-fg, &status, WUNTRACED | WCONTINUED);
              if (pid == -1)
                // There are still processes that are not terminated in the fg pg
                if (errno == ECHILD)
                  break;

              // Process was interrupted by Ctrl+Z
              if (WIFSTOPPED(status))
                break;
            }
            output = -1;
            input = 0;
            fg = 0;
          }
        }
        else
        {
          if (!bgpg)
            bgpg = pid;
          setpgid(pid, bgpg);
        }
        tcsetpgrp(0, getpid());
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