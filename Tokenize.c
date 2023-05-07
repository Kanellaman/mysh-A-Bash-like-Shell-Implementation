#include "Interface.h"

/* Parsing,tokenization and dividing commands */
char **tokenize(char *str)
{ /* This functions gets a string and tokenizes it to tokens whcih are stored at the variable tokens */
    int i = 0, last = 0, j = 0, count = 0;
    char *cp, **tokens = malloc(TOKEN_NUM * sizeof(char *));
    for (int i = 0; i < TOKEN_NUM; i++)
        tokens[i] = NULL;
    bool flag = false;
    char *copy = malloc(strlen(str) * sizeof(char) + 1); // A copy of user's input
    strcpy(copy, str);

    /* Bad syntax-print error */
    for (int i = 0; i < strlen(copy); i++)
        if (copy[i] == '\"')
            count++;
    if (count % 2 != 0)
    {
        printf("There is \" that never closes\nSyntax error\n");
        free(copy);
        free(tokens);
        return NULL;
    }
    /* Deviding the input to spaces */
    cp = strtok(copy, " \n");
    if (cp == NULL)
    { /* In case of empty input containing spaces and \n ask the next input */
        free(cp);
        free(copy);
        return NULL;
    } /* In case of " " get the whole " " string as it is */
    else if (cp[0] == '"')
        tokens = quote(cp, tokens, &i);
    else /* If there are no spaces between <>|&; this function will find them and make them separate tokens */
    {
        tokens = custom_tokenize(cp, tokens, &i, &last, &j, &flag);
        tokens[i] = malloc((j - last) * sizeof(char) + 1);
        strncpy(tokens[i], cp + last, j - last);
        tokens[i][j - last] = '\0';
        if (!strlen(tokens[i])) /* If the last token is a character <,>,|... */
        {
            free(tokens[i]);
            tokens[i--] = NULL;
        }
    }
    /* Repeat the above for every word divided by spaces with the others */
    while (cp != NULL)
    {
        last = 0;
        i++;
        cp = strtok(NULL, " \n");
        if (cp == NULL)
            break;
        else if (cp[0] == '"')
            tokens = quote(cp, tokens, &i);
        else /* If there are no spaces between <>|&; this function will find them and make them separate tokens */
        {
            tokens = custom_tokenize(cp, tokens, &i, &last, &j, &flag);
            tokens[i] = malloc((j - last) * sizeof(char) + 1);
            strncpy(tokens[i], cp + last, j - last);
            tokens[i][j - last] = '\0';
            if (!strlen(tokens[i]))
            {
                free(tokens[i]);
                tokens[i--] = NULL;
            }
        }
    }
    /* Free reserved memory and search for wild characters to replace */
    free(cp);
    free(copy);
    return wild(tokens);
}
char **custom_tokenize(char *cp, char **tokens, int *i, int *last, int *j, bool *flag)
{
    int x, y = *i;
    for (x = 0; x < strlen(cp); x++)
    { /* Search the word for special characters like <>|&; */
        if (cp[x] == '>' && x + 1 < strlen(cp) && cp[x + 1] == '>')
        { /* If you find >> ... */
            char *s = ">>";

            if (x != 0)
            { /* last variable points to next char of the last character of the string cp we added to a token */
                tokens[y] = malloc((x - *last) * sizeof(char) + 1);
                strncpy(tokens[y++], cp + *last, x - *last); // Add as a token the characters from the last one inserted(last) till the character '>'
                tokens[y - 1][x - *last] = '\0';
            }
            tokens[y] = malloc(sizeof(char) + 1);
            strcpy(tokens[y++], s); // Store the special character as an individual token
            x++;
            *last = x + 1;
        }
        else if (cp[x] == '<' || cp[x] == '>' || cp[x] == '|' || cp[x] == '&' || cp[x] == ';')
        {
            char s[2];
            s[0] = cp[x];
            s[1] = '\0';
            if (x != 0 && *last != x)
            { /* last variable points to next char of the last character of the string cp we added to a token */
                tokens[y] = malloc((x - *last) * sizeof(char) + 1);
                strncpy(tokens[y++], cp + *last, x - *last); // Add as a token the characters from the last one inserted(last) till the special character you found
                tokens[y - 1][x - *last] = '\0';
            }
            tokens[y] = malloc(sizeof(char) + 1);
            strcpy(tokens[y++], s); // Store the special character as an individual token
            *last = x + 1;
        }
    }
    *j = x;
    *i = y;
    return tokens;
}
char **wild(char **tokens)
{ /* Replace wild characters and enviroment variables */
    glob_t globbuf;
    int flags = GLOB_NOCHECK | GLOB_TILDE, j = 0;
    char **args = malloc(TOKEN_NUM * sizeof(char *));
    for (int i = 0; i < TOKEN_NUM; i++)
        args[i] = NULL;
    for (int i = 0; tokens[i] != NULL; i++)
    {
        if (strchr(tokens[i], '*') != NULL || strchr(tokens[i], '?') != NULL)
        { /* If there is a wild character call glob to find its matches and copy them to the new args array */

            if (glob(tokens[i], 0, NULL, &globbuf) == 0)
            {
                for (size_t i = 0; i < globbuf.gl_pathc; i++)
                {
                    args[j] = malloc(strlen(globbuf.gl_pathv[i]) * sizeof(char) + 1);
                    strcpy(args[j++], globbuf.gl_pathv[i]);
                }
                globfree(&globbuf);
                free(tokens[i]);
            }
        }
        else if (tokens[i][0] == '$')
        { /* If you find enviroment variables replace them */
            char *str = malloc(LINE_SIZE);
            strncpy(str, tokens[i] + 1, strlen(tokens[i]));
            str[strlen(str)] = '\0';
            args[j] = malloc(strlen(str) * sizeof(char) + 1);
            strcpy(args[j++], getenv(str));
            free(str);
        }
        else
            args[j++] = tokens[i];
    }
    free(tokens);
    return args;
}
char **quote(char *cp, char **tokens, int *i)
{
    tokens[*i] = malloc(LINE_SIZE * sizeof(char) + 1);
    strcpy(tokens[*i], cp);
    if (cp[strlen(cp) - 1] != '\"')
    { /* Make sure you get everything between " and " */
        cp = strtok(NULL, "\"");
        if (cp != NULL)
        {
            if (cp[0] == '\n')
                cp[0] = ' ';
            else
                strcat(tokens[*i], " ");

            strcat(tokens[*i], cp);
            strcat(tokens[*i], "\"");
        }
    }
    if (tokens[*i][strlen(tokens[*i]) - 1] != '\"')
    {
        char ch = tokens[*i][strlen(tokens[*i]) - 1];
        tokens[*i][strlen(tokens[*i]) - 1] = '\0';
        tokens[++(*i)] = malloc(sizeof(char) + 1);
        sprintf(tokens[*i], "%c", ch);
    }
    return tokens;
}
char **cleanup(char **tokens)
{                                                     /* Search the tokens for  */
    char **args = malloc(TOKEN_NUM * sizeof(char *)); // args variables contains the filtered arguments and not redirections!

    for (int i = 0; i < TOKEN_NUM; i++)
        args[i] = NULL;
    int i = 0, x = 0;
    while (tokens[i] != NULL)
    {
        if (!strcmp(tokens[i], "<") || !strcmp(tokens[i], ">") || !strcmp(tokens[i], ">>"))
        { /* Filter out the tokens with redirections and the files to which are redirecting to */
            i += 2;
            continue;
        }
        if (!strcmp(tokens[i], ";") || !strcmp(tokens[i], "|") || !strcmp(tokens[i], "&"))
        { /* Filter out also the tokens which indicate fg/bg/pipe command */
            i++;
            continue;
        }
        if (tokens[i][0] == '\"')
        { /* If a token has " " then remove the " " */
            args[x] = malloc(strlen(tokens[i]) * sizeof(char) - 1);
            strncpy(args[x], tokens[i] + 1, strlen(tokens[i]) - 2);
            args[x++][strlen(tokens[i++]) - 1] = '\0';
            continue;
        }
        args[x] = malloc(strlen(tokens[i]) * sizeof(char) + 1);
        strcpy(args[x++], tokens[i++]); // Useful arguments that dont need filtering
    }
    return args; // Return the new tokens-arguments for the command
}
char ***separate(char ***tok, char **tokens, int *total)
{ /* Check if the user has given multiple commands to one line */
    int p = 0, pipes = 0, pun = 0, amper = 0;
    while (tokens[p] != NULL)
    {
        if (!strcmp(tokens[p], "|"))
            pipes++;
        else if (!strcmp(tokens[p], ";"))
            pun++;
        else if (!strcmp(tokens[p], "&"))
        {
            if (tokens[p + 1] != NULL && !strcmp(tokens[p + 1], ";"))
                pun--;
            amper++;
        }
        p++;
    }
    *total = pipes + pun + amper;

    if (*total > 0)
    { /* If there are more than 1 commands */

        tok = malloc(sizeof(char **));
        int j = 0, x = 0;
        tok[0] = malloc(sizeof(char *));
        for (int i = 0; i < TOKEN_NUM && tokens[i] != NULL; i++)
        { /* Traversing each element of an array so that devide each command to a different element of the array tok */
            int flag = 1;
            if (!strcmp(tokens[i], "|") || !strcmp(tokens[i], ";") || !(flag = strcmp(tokens[i], "&")))
            { /* If you find |;& change command slot in array tok */
                tok[j][x] = tokens[i];
                tok[j] = realloc(tok[j], (2 + x) * sizeof(char **));
                tok[j][++x] = NULL;
                if (tokens[i + 1] == NULL)
                    break;
                if (!flag && !strcmp(tokens[i + 1], ";")) // flag tells us if the current token is & or not
                {                                         /* Ignore ; after & as the commands &; and & are equal */
                    i++;
                    free(tokens[i]);
                    if (tokens[i + 1] == NULL)
                        break;
                }
                x = 0;
                j++;
                tok = realloc(tok, (j + 1) * sizeof(char **));
                tok[j] = malloc(sizeof(char *));
                continue;
            } /* Store the tokens that belong to the same command together */
            tok[j][x] = tokens[i];
            tok[j] = realloc(tok[j], (2 + x) * sizeof(char **));
            tok[j][++x] = NULL;
        }
        *total = j + 1;
        return tok;
    }
    return NULL;
}
