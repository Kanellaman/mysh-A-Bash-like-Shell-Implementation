#include "../Include/Interface.h"
/* Signals! */
void sig_handler(int sig)
{
    if (fg)
        killpg(-fg, sig);
    fg = 0;
}
void signals(struct sigaction *sa, struct sigaction *as)
{
    sa->sa_handler = sig_handler;
    sa->sa_flags = SA_SIGINFO;
    sa->sa_flags = SA_RESTART;
    sigemptyset(&sa->sa_mask);
    as->sa_handler = SIG_IGN;
    as->sa_flags = SA_SIGINFO;
    as->sa_flags = SA_RESTART;
    sigemptyset(&as->sa_mask);
    /* Handle the signals through the sig_handler function */
    if (sigaction(SIGINT, sa, NULL) == -1)
    {
        perror("sigaction for SIGINT");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGTSTP, sa, NULL) == -1)
    {
        perror("sigaction for SIGTSTP");
        exit(EXIT_FAILURE);
    }

    /* Ignore these signals so that we give the input/output to the process group we want later on */
    if (sigaction(SIGTTOU, as, NULL) == -1)
    {
        perror("sigaction for SIGTTOU");
        exit(EXIT_FAILURE);
    }

    if (sigaction(SIGTTIN, as, NULL) == -1)
    {
        perror("sigaction for SIGTTIN");
        exit(EXIT_FAILURE);
    }
    setpgid(0, getpid());   // Create a process group that contains only the shell
    tcsetpgrp(0, getpid()); // Set as foreground process group the shell
}

/* Commands implementation(cd/redirection/history/aliases) */
int cd(char **tokens)
{ /* Support of cd command */
    char *cd_loc = tokens[1];
    if (cd_loc == NULL || (!strcmp(cd_loc, "&") || !strcmp(cd_loc, "|") || !strcmp(cd_loc, ";")))
        cd_loc = getenv("HOME");
    int error = chdir(cd_loc);
    return error;
}

int redirection(char **tokens)
{ /* If there is a redirection,do it */
    int i = 0, fd, dsc;
    while (tokens[i] != NULL)
    { /* Search all tokens for redirections */
        if (!strcmp(tokens[i], "<") || !strcmp(tokens[i], ">") || !strcmp(tokens[i], ">>"))
        {
            dsc = STDOUT_FILENO;        // This will later change if we have <
            char *file = tokens[i + 1]; // Get the name of the file
            if (file == NULL)
            { /* If the file does not exist return error */
                printf("Missing file for redirection\n");
                return -1;
            }
            if (!strcmp(tokens[i], "<"))
            { /* Open the file only for read */
                dsc = STDIN_FILENO;
                fd = open(file, O_RDONLY);
            }
            else if (!strcmp(tokens[i], ">")) // Open the file for write/truncate and if it does not exists create it with the suitable permitions
                fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, 0644);
            else if (!strcmp(tokens[i], ">>")) // Open the file for write/append and if it does not exists create it with the suitable permitions
                fd = open(file, O_WRONLY | O_APPEND | O_CREAT, 0644);
            if (fd == -1)
            { /* If an error comes up while opening a file print error */
                perror("open");
                return -1;
            }
            close(dsc);
            if (dup2(fd, dsc) == -1)
            { /* Redirect the input/output to the file you opened */
                perror("dup");
                return -1;
            }
            close(fd);
        }
        i++;
    }
    // return 0;
}

int hs_al(char **tokens, ptr *hs, alr *al, char **str)
{ /* Handle alias and myhistory commands */
    alr alias;
    int num;
    if (!strcmp(tokens[0], "myhistory"))
    {
        if (tokens[1] == NULL || (!strcmp(tokens[1], "|") || !strcmp(tokens[1], ";") || !strcmp(tokens[1], "&")))
            print((*hs)); // Just print the myhistory!
        else if (sscanf(tokens[1], "%d", &num) == 1)
        { /* return the myhistory <num> myhistory command-if exists */
            if (num > 0 && num <= (*hs)->count - 1)
            {
                /* Get the num-th command to return it */
                strcpy(*str, get_command(*hs, num));
                if ((*str)[strlen(*str) - 1] == '\n')
                    (*str)[strlen(*str) - 1] = '\0'; /* Filter out the \n character */
                int i = 2;
                while (tokens[i] != NULL)
                { /* Add the rest of the arguments given to the new command */
                    strcat(*str, " ");
                    strcat(*str, tokens[i++]);
                }
                strcat(*str, "\n");
                return num;
            }
            else /* Not valid command number */
                printf("There is no %dth command in myhistory...Try command \"myhistory\" to see the command myhistory\n", num);
        }
        else /* Bad syntax error message */
            printf("Bad syntax of command \"myhistory\".\nTry \"myhistory\" or \"myhistory <num>\" where num is in number in the range of 1..20\n");
    }
    else if (!strcmp(tokens[0], "createalias"))
    { /* Create an alias */
        if (tokens[1] != NULL && tokens[2] != NULL && tokens[2][0] == '\"' && (tokens[3] == NULL || (!strcmp(tokens[3], "|") || !strcmp(tokens[3], ";") || !strcmp(tokens[3], "&"))))
        {
            alr tmp = NULL;
            char *cmd = malloc((LINE_SIZE / 2));
            strncpy(cmd, tokens[2] + 1, strlen(tokens[2]) - 2);
            cmd[strlen(tokens[2]) - 1] = '\0';
            tmp = in(*al, tokens[1], cmd);
            if (tmp != NULL)
                *al = tmp;
            free(cmd);
        }
        else /* Syntax error message */
            printf("Bad syntax of command \"createalias\".\nTry createalias <alias> \"command to alias\";\n");
    }
    else if (!strcmp(tokens[0], "destroyalias"))
    { /* Destroy an alias */
        if (tokens[1] != NULL && (tokens[2] == NULL || (!strcmp(tokens[2], "|") || !strcmp(tokens[2], ";") || !strcmp(tokens[2], "&"))))
            *al = delal(*al, tokens[1]);
        else /* Syntax error message */
            printf("Bad syntax of command \"destroyalias\".\nTry destroyalias <alias>;\n");
    }
    else if ((alias = search(*al, tokens[0])) != NULL)
    { /* Check if command is an alias for an other command */
        strcpy(*str, alias->cmd);
        strcat(*str, " ");
        int i = 1;
        while (tokens[i] != NULL)
        { /* Copy the arguments of alias command to the actual command */
            strcat(*str, tokens[i++]);
            strcat(*str, " ");
        }
        strcat(*str, "\n");
        return 0;
    }
    else if (!strcmp(tokens[0], "alias"))
    {
        alr tmp = *al;
        while (tmp != NULL)
        {
            printf("alias %s=%s\n", tmp->alias, tmp->cmd);
            tmp = tmp->next;
        }
    }
    else
        return -2;
    return -1;
}

/* General-use */
char ***frees(char ***tok, int total)
{ /* Free the table we keep the tokens */
    if (tok != NULL)
    {
        for (int j = 0; j < total; j++)
            if (tok[j] != NULL)
            {
                for (int i = 0; i < TOKEN_NUM; i++)
                    if (tok[j][i] != NULL)
                    {
                        free(tok[j][i]);
                        tok[j][i] = NULL;
                    }
                    else
                        break;
                free(tok[j]);
                tok[j] = NULL;
            }
        free(tok);
        tok = NULL;
    }
    return tok;
}
