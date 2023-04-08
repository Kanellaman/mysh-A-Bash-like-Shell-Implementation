#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(char *argc, char **argv)
{
  int opt;

  while ((opt = getopt(argc, argv, "abc")) != -1)
  {
    switch (opt)
    {
    case 'a':
      printf("Option -a was specified\n");
      break;
    case 'b':
      printf("Option -b was specified\n");
      break;
    case 'c':
      printf("Option -c was specified\n");
      break;
    default:
      printf("Invalid option\n");
      exit(EXIT_FAILURE);
    }
  }
  return 1;
}