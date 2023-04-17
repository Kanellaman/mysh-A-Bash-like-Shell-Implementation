#include "Interface.h"

ptr init(ptr hs)
{
    hs->count = 0;
    return hs;
}

int valid(char *str)
{
    return 1;
}

void tokenize(char *copy, char ***tokens)
{
    int i = 0, last = 0;
    char *cp;
    cp = strtok(copy, " \n");
    (*tokens)[i] = cp;

    // for (int j = 0; j < strlen(cp); j++)
    // {
    //     if (cp[j] == '<')
    //     {
    //         char *tmp = malloc(strlen(cp) * sizeof(char) + 1);
    //         strcpy(tmp, cp);
    //         tokens[++i] = strtok(tmp, "<");
    //         tokens[++i] = "<";
    //         tokens[++i] = strtok(NULL, "<");
    //     }
    // }
    while ((*tokens)[i] != NULL)
    {
        i++;
        cp = strtok(NULL, " \n");
        (*tokens)[i] = cp;
    }
}

int cd(char **tokens)
{
    char *cd_loc = tokens[1];
    if (cd_loc == NULL)
        cd_loc = getenv("HOME");
    int error = chdir(cd_loc);
    return error;
}

int find(char **tokens, char *redir)
{
    int i = 0;
    while (tokens[i] != 0)
        if (!strcmp(tokens[i++], redir))
            return --i;
    return -1;
}

int redirection(char **tokens, char *redir, char *str, char *copy)
{
    int i, fd, dsc;
    if ((i = find(tokens, redir)) != -1)
    {
        char *file = tokens[i + 1];
        if (file == NULL)
        {
            printf("Missing file for redirection");
            return -1;
        }
        if (!strcmp(redir, "<"))
        {
            dsc = STDIN_FILENO;
            fd = open(file, O_RDONLY);
        }
        else if (!strcmp(redir, ">"))
        {
            dsc = STDOUT_FILENO;
            fd = open(file, O_WRONLY | O_TRUNC | O_CREAT);
        }
        else
        {
            dsc = STDOUT_FILENO;
            fd = open(file, O_WRONLY | O_APPEND | O_CREAT);
        }
        if (fd == -1)
            frees(copy, str, tokens, "open");
        close(dsc);
        if (dup2(fd, dsc) == -1)
            frees(copy, str, tokens, "dup2");
        close(fd);
    }
    return 0;
}

void frees(char *str, char *copy, char **tokens, char *s)
{
    perror(s);
    free(str);
    free(copy);
    free(tokens);
    exit(EXIT_FAILURE);
}