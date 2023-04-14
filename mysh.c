#include "Interface.h"

int main(char *argc, char **argv)
{
  ptr hs;
  init(hs);
  int opt;
  char *str = malloc(LINE_SIZE), *copy, **tokens;
  printf("in-mysh-now:>");
  int i = 0;
  while (fgets(str, LINE_SIZE, stdin) != NULL && i == 0)
  {
    tokens = tokenize(str);

    /* while (tokens[i] != NULL)
    {
      printf("%s\n", tokens[i++]);
    } */
    printf("in-mysh-now:> ");
  }
  return 0;
}