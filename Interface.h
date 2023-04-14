#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define LINE_SIZE 1024

typedef struct history *ptr;
struct history
{
    char *command[20];
    struct history *next;
};

ptr init(ptr hs);
int valid(char *str);
char **tokenize(char *str);