#include <stdio.h>
#include <stdlib.h>
int main()
{
  printf("Hello\n");

  char *home_dir = getenv("HOME");
  if (home_dir != NULL)
  {
    printf("Home directory is %s\n", home_dir);
  }
  else
  {
    printf("Home directory not found\n");
  }
  return 0;
}