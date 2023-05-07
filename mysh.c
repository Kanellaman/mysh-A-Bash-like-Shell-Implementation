#include "Interface.h"

pid_t fg = 0;

int main(char *argc, char **argv)
{
  struct sigaction sa, as;
  char *str = malloc(LINE_SIZE), **tokens = NULL, ***tok = NULL;
  int pid, i = 1, num = -5, total = 0, status;
  ptr hs = NULL;
  alr al = NULL;
  fg = 0;
  /* Handle some signals */
  signals(&sa, &as);

  printf("in-mysh-now:> ");
  while (i != 0 && fgets(str, LINE_SIZE, stdin) != NULL)
  {
    pid_t bgpg = 0;
    int child;
    while ((child = waitpid(-1, &status, WNOHANG)) > 0)
      printf("\n            %d Done!\n\n", child);
    tok = frees(tok, total); // Free the memory used for the previous commands
    total = 0;               // This is the variable that stores the number of commands the user gave
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
    /* Separate each command to tokens */
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
      /* Search for aliases or myhistory commands */
      while ((num = hs_al(tok[j], &hs, &al, &str)) >= 0)
      {
        strcpy(prev, tok[j][0]); // Keeping the first token so that we dont end up to an infinite loop of aliases or myhistory commands
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
        if (num > 0 && total == 1) // In case we have only one command and it coreesponds to "myhistory <num>" dont insert it to the myhistory
          hs = append(hs, str);
        /* Tokenize the new command so that it is ready to be executed */
        tok[j] = tokenize(str);
        if (!strcmp(prev, tok[j][0]) && strcmp(prev, "myhistory"))
        { // If previous first token is the same with the new one then stop searching for aliases/myhistory commands
          num = -2;
          break;
        }
        strcpy(prev, tok[j][0]);
      }
      free(prev); // Free memory
      if (!strcmp(tok[j][0], "exit"))
      { // Exit the shell
        printf("You are exiting mysh!\n");
        i = 0;
        break;
      }
      else if (!strcmp(tok[j][0], "cd"))
      { // Perform cd
        if (cd(tok[j]) != 0)
          perror("cd");
        continue;
      }
      if (num == -1)
        continue;
      int last = -1, status;
      pid_t pid, pgid;
      while (tok[j][++last] != NULL) // Set last - 1 to point to the last element of the current command
        ;
      if (!strcmp(tok[j][last - 1], "|"))
      { /* Make a pipe */
        pipe(fds);
        output = fds[1]; /* Set as the output of the command the fds[1] */
      }
      else if (output > -1)
        output = 1; /* Reset the output if this is the last command of the pipe */
      pid = fork();

      if (!strcmp(tok[j][last - 1], "|") && back == -1) // Only the first command of the pipe will execute this!
      {                                                 // Handle background pipeline
        for (int i = j + 1; i < total; i++)
        {
          int last = -1;
          /* Check the last token of every command */
          while (tok[i][++last] != NULL)
            ;
          if (!strcmp(tok[i][last - 1], "|"))
            continue;
          if (!strcmp(tok[i][last - 1], ";"))
            break;
          if (!strcmp(tok[i][last - 1], "&"))
          { /* If the piping is background */
            back = 1;
            /* Set the process id of the first pipeline command as the process group leader */
            if (pid == 0)
              bgpg = getpid();
            else
              bgpg = pid;
          }
        }
      }

      if (pid == 0)
      {
        if (input) // If there is piping
        {          /* Determine the input of the child */
          dup2(input, 0);
          close(input);
        }
        if (output != 1 && output != -1) // If there is piping
        {                                /* Determine the output of the child */
          dup2(output, 1);
          close(output);
          close(fds[0]);
        }
        if (strcmp(tok[j][last - 1], "&"))
        {
          if (!fg)
            fg = pid;
          setpgid(getpid(), fg);
          tcsetpgrp(0, fg); // Set as foreground process group the commands that are now running
        }
        else if (back == 1)
          setpgid(getpid(), bgpg); // Add the process to the background pipe process group
        else
          setpgid(getpid(), 0); // Add each bg command to its own process group

        if (redirection(tok[j]) == -1) // If there are redirections then make them happen!
          return 0;
        signal(SIGTTIN, SIG_DFL);
        signal(SIGTTOU, SIG_DFL);
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
        /* If a command runs in the background print its pid */
        if (!strcmp(tok[j][last - 1], "&") || back == 1)
          printf("[Running in background] %d\n", pid);

        /* Close the fds if they have been openned */
        if (output != 1 && output != -1)
          close(output);
        if (input != 0)
          close(input);

        if (!strcmp(tok[j][last - 1], "|"))
          input = fds[0]; // Set the input for the next command of the pipe
        if (strcmp(tok[j][last - 1], "&"))
        { // Foreground command(s) including cases of pipeline
          if (!fg)
            fg = pid;
          setpgid(pid, fg); // Add the pid child to the foreground pg
          tcsetpgrp(0, fg); // Set as foreground process group the commands that are now running
          if (strcmp(tok[j][last - 1], "|"))
          {
            while (1)
            { // Wait all the fg processes
              pid_t pid = waitpid(-fg, &status, WUNTRACED | WCONTINUED);
              if (pid == -1)
                // There are no more processe in the fg
                if (errno == ECHILD)
                  break;

              // Process was interrupted by Ctrl+Z
              if (WIFSTOPPED(status))
                break;
            }
            /* Reset the foregroud "fg" variable and the variables output,input */
            output = -1;
            input = 0;
            fg = 0;
          }
        }
        else if (back == 1)
          setpgid(pid, bgpg); // Add the process to the background pipe process group
        else
          setpgid(pid, 0);      // Add each bg command to its own process group
        tcsetpgrp(0, getpid()); // Set as foreground process group the shell
      }
      if (strcmp(tok[j][last - 1], "|") && back != -1)
      { /* Reset the (pipeline) background "back" flag and the variables output,input */
        back = -1;
        output = -1;
        input = 0;
      }
    }
    if (i != 0)
      printf("in-mysh-now:> ");
  }
  /* Free the memory you used */
  tok = frees(tok, total);
  free(str);
  del(hs);
  dele(al);
  return 0;
}