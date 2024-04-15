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

#define LINE_SIZE 1024 /* Max number of characters per command */
#define TOKEN_NUM 50   /* Max number of individual tokens per command */
extern pid_t fg;       /* Stores the pid-leader of the foreground process group */
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
    char alias[LINE_SIZE / 2], cmd[LINE_SIZE / 2];
    struct alias *next;
};

/* Signals! */
void sig_handler(int sig);
void signals(struct sigaction *sa, struct sigaction *as);

/* Parsing,tokenization and dividing commands */
char **tokenize(char *str);
char **custom_tokenize(char *cp, char **tokens, int *i, int *last, int *j, bool *flag);
char **wild(char **tokens);
char **quote(char *cp, char **tokens, int *i);
char **cleanup(char **tokens);
char ***separate(char ***tok, char **tokens, int *total);

/* Commands implementation(cd/redirection/history/aliases) */
int cd(char **tokens);
int redirection(char **tokens);
int hs_al(char **tokens, ptr *hs, alr *al, char **str);

/* History handling */
ptr append(ptr hs, char *str);
void print(ptr hs);
void del(ptr hs);
char *get_command(ptr hs, int num);

/* Aliases handling */
alr delal(alr al, char *alias);
alr in(alr al, char *alias, char *cmd);
alr search(alr al, char *alias);
void dele(alr al);

/* General-use */
char ***frees(char ***tokens, int total);