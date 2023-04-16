#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#define LINE_SIZE 1024

typedef struct history *ptr;
struct history
{
    char *command[20];
    struct history *next;
};

void frees(char *str,char *copy, char **tokens,char* s);
ptr init(ptr hs);
int valid(char *str);
char **tokenize(char *str);
int cd(char **tokens);
int redirection(char **tokens);