#include "Interface.h"

ptr init(ptr hs)
{
    return hs;
}

int valid(char *str)
{
    return 1;
}

char **tokenize(char *str)
{
    char *copy = malloc(strlen(str) * sizeof(char) + 1);
    char **tokens = malloc(20 * sizeof(char *));
    int i = 0;

    strcpy(copy, str);
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