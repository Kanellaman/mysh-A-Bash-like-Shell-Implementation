#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>

#define LINE_SIZE 1024

typedef struct history *ptr;
struct history
{
    int count;
    char *command[20];
    struct history *next;
};

void frees(char *str, char *copy, char **tokens, char *s);
ptr init(ptr hs);
int valid(char *str);
void tokenize(char *str,char ***tokens);
int cd(char **tokens);
int find(char **tokens, char *redir);
int redirection(char **tokens, char *redir, char *str, char *copy);