#include "Interface.h"

int main(char *argc, char **argv)
{
  ptr hs;
  init(hs);
  int opt;
  char *str = malloc(LINE_SIZE), **tokens;
  printf("in-mysh-now:> ");
  int i = 0, pid;
  while (fgets(str, LINE_SIZE, stdin) != NULL)
  {
    tokens = tokenize(str);

    if (!strcmp(tokens[0], "exit"))
    {
      printf("You are exiting mysh!\n");
      break;
    }

    if (!strcmp(tokens[0], "cd"))
    {
      char *cd_loc = tokens[1];
      if (cd_loc == NULL)
        cd_loc = getenv("HOME");
      int error = chdir(cd_loc);
      if (error != 0)
      {
        perror("chdir");
        exit(EXIT_FAILURE);
      }
    }
    else
    {
      pid = fork();
      if (pid == 0)
      {
        int error = execvp(tokens[0], tokens);
        if (error == -1)
        {
          perror("Execvp");
          exit(0);
        }
      }
      else
      {
        wait(NULL);
      }
    }

    printf("in-mysh-now:> ");
  }
  return 0;
}