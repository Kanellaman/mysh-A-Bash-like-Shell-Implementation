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
    char command[LINE_SIZE];
    struct history *next;
};

void frees(char *str, char *copy, char **tokens, char *s);
ptr init(ptr hs);
int valid(char *str);
char **tokenize(char *str);
char **custom_tokenize(char *cp, char **tokens, int *i, int *last, int *j);
int cd(char **tokens);
int find(char **tokens, char *redir);
int redirection(char **tokens, char *redir, char *str, char *copy);
ptr append(ptr hs,char *str);
ptr print(ptr hs);
void del(ptr hs);