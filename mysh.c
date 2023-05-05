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
  struct sigaction sa, as;
  sa.sa_handler = sig_handler;
  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  as.sa_handler = SIG_IGN;
  as.sa_flags = SA_SIGINFO;
  sigemptyset(&as.sa_mask);
  int k = 5;
  char *str = malloc(LINE_SIZE), **tokens = NULL, ***tok = NULL;
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

  /* Ignore these signals so that we give the input/output to the process group we want later on */
  // signal(SIGTTOU, SIG_IGN);
  // signal(SIGTTIN, SIG_IGN);
  if (sigaction(SIGTTOU, &as, NULL) == -1)
  {
    perror("sigaction for SIGTTOU");
    exit(EXIT_FAILURE);
  }

  if (sigaction(SIGTTIN, &as, NULL) == -1)
  {
    perror("sigaction for SIGTTIN");
    exit(EXIT_FAILURE);
  }
  setpgid(0, getpid());   // Create a process group that contains only the shell
  tcsetpgrp(0, getpid()); // Set as foreground process group the shell
  printf("in-mysh-now:> ");

  while (i != 0 && fgets(str, LINE_SIZE, stdin) != NULL)
  {
    pid_t bgpg = 0;

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
    int p = -1;
    while (tokens[++p] != NULL)
      ;
    if (!strcmp(tokens[p - 1], "|"))
    {
      printf("Expected command after last token\n");
      printf("in-mysh-now:> ");
      continue;
    }
    tok = separate(tok, tokens, &total);
    if (tok == NULL)
    {
      total = 1;
      tok = malloc(sizeof(char **));
      tok[0] = tokens;
    }
    else
      free(tokens);
    int output = -1, input = 0, fds[2], back = -1;
    for (int j = 0; j < total; j++)
    {
      char *prev = malloc(TOKEN_NUM);
      while ((num = hs_al(tok[j], &hs, &al, &str)) >= 0)
      {
        strcpy(prev, tok[j][0]); // Keeping the first token so that we dont end up to an infinite loop of aliases or history commands
        /* Freeing the memory for the last command as we no longer need it */
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
        if (num > 0 && total == 1) // In case we have only one command and it coreesponds to "history <num>" dont insert it to the history
          hs = append(hs, str);
        tok[j] = tokenize(str);
        if (!strcmp(prev, tok[j][0]))
        { // If previous first token is the same with the new one then stop searching for aliases/history commands
          num = -2;
          break;
        }
        strcpy(prev, tok[j][0]);
      }
      free(prev);
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
      if (!strcmp(tok[j][last - 1], "|") && back == -1)
      { // Handle background pipeline
        for (int i = j + 1; i < total; i++)
        {
          int last = -1;
          while (tok[i][++last] != NULL)
            ;
          if (!strcmp(tok[i][last - 1], "|"))
            continue;
          if (!strcmp(tok[i][last - 1], ";"))
            break;
          if (!strcmp(tok[i][last - 1], "&"))
          {
            back = 1;
            if (pid == 0)
              bgpg = getpid();
            else
              bgpg = pid;
          }
        }
      }

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
        if (strcmp(tok[j][last - 1], "&") && back == -1)
        {
          if (!fg)
            fg = pid;
          setpgid(getpid(), fg);
          tcsetpgrp(0, fg); // Set as foreground process group the commands that are now running
        }
        else if (back == 1)
          setpgid(pid, bgpg);
        else
          setpgid(pid, 0);

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
        if (strcmp(tok[j][last - 1], "&") && back == -1)
        { // Foreground command(s) including cases of pipeline
          if (!fg)
            fg = pid;
          setpgid(pid, fg);
          tcsetpgrp(0, fg); // Set as foreground process group the commands that are now running
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
        else if (back == 1) // Background pipeline
          setpgid(pid, bgpg);
        else // Just a background command
          setpgid(pid, 0);
        tcsetpgrp(0, getpid()); // Set as foreground process group the shell
      }
      if (strcmp(tok[j][last - 1], "|") && back != -1)
        back = -1; // Reset the (pipeline) background "back" flag
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