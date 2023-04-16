#include "Interface.h"

ptr init(ptr hs)
{
    return hs;
}

int valid(char *str)
{
    return 1;
}

char **tokenize(char *copy)
{
    char **tokens = malloc(20 * sizeof(char *));
    int i = 0;

    tokens[i] = strtok(copy, " \n/");
    while (tokens[i] != NULL)
    {
        i++;
        tokens[i] = strtok(NULL, " \n/");
    }
    return tokens;
}

int cd(char **tokens)
{
    char *cd_loc = tokens[1];
    if (cd_loc == NULL)
        cd_loc = getenv("HOME");
    int error = chdir(cd_loc);
    return error;
}

int redirection(char **tokens)
{
    int i = 0;
    while (tokens[i] != 0)
        if (!strcmp(tokens[i++], "<"))
            return --i;
    return -1;
}

void frees(char *str, char *copy, char **tokens, char *s)
{
    perror(s);
    free(str);
    free(copy);
    free(tokens);
    exit(EXIT_FAILURE);
}