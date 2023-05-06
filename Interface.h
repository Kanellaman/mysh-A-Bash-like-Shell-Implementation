#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdbool.h>
#include <glob.h>
#include <signal.h>
#include <sys/types.h>
#include <errno.h>

#define LINE_SIZE 1024
#define TOKEN_NUM 50

typedef struct history *ptr;
typedef struct alias *alr;
struct history
{
    int count;
    char command[LINE_SIZE];
    struct history *next;
};
struct alias
{
    char alias[LINE_SIZE/2],cmd[LINE_SIZE/2];
    struct alias *next;
};
char ***frees(char ***tokens,int total);
char **tokenize(char *str);
char **custom_tokenize(char *cp, char **tokens, int *i, int *last, int *j,bool *flag);
int cd(char **tokens);
int find(char **tokens, char *redir);
int redirection(char **tokens);
ptr append(ptr hs,char *str);
void print(ptr hs);
void del(ptr hs);
char *get_command(ptr hs, int num);
alr delal(alr al,char *alias);
alr in(alr al, char *alias, char *cmd);
alr search(alr al, char *alias);
void dele(alr al);
int hs_al(char **tokens, ptr *hs, alr *al,char **str);
char **wild(char **tokens);
char **cleanup(char **tokens);
char ***separate(char ***tok,char **tokens,int *total);
char **quote(char *cp,char **tokens,int *i);