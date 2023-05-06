#include "Interface.h"
char *get_command(ptr hs, int num)
{ /* Returns the i-th command of the history */
    int i = 1;
    while (hs->next != NULL)
    {
        if (i++ == num)
            return hs->command;
        hs = hs->next;
    }
    if (i == num)
        return hs->command;
}
void del(ptr hs)
{ /* Free the memory that history uses */
    ptr prev;
    if (hs == NULL)
        return;
    while (hs->next != NULL)
    {
        prev = hs;
        hs = hs->next;
        free(prev);
    }
    free(hs);
}
ptr append(ptr hs, char *str)
{ /* Add a command to the history */
    if (hs == NULL)
    {
        hs = malloc(sizeof(struct history));
        hs->count = 1;
        strcpy(hs->command, str);
        hs->next = NULL;
        return hs;
    }
    ptr head = hs;

    while (hs->next != NULL)
        hs = hs->next;
    if (!strcmp(str, hs->command)) // If you try to insert the same command as the last one in history, don't
        return head;

    head->count = head->count + 1;
    if (head->count == 21)
    {
        hs = head->next;
        hs->count = 20;
        free(head);
        head = hs;
    }
    while (hs->next != NULL)
        hs = hs->next;
    hs->next = malloc(sizeof(struct history));
    strcpy(hs->next->command, str);
    hs->next->count = 0;
    hs->next->next = NULL;
    return head; // Return a pointer to the first command
}
void print(ptr hs)
{ /* Print the history */
    int i = 1;
    if (hs == NULL)
        return;
    while (hs->next != NULL)
    {
        printf("%d: %s", i++, hs->command);
        hs = hs->next;
    }
    printf("%d: %s", i, hs->command);
}
alr in(alr al, char *alias, char *cmd)
{ /* Add an alias */
    alr head = al, tmp;
    if (al == NULL)
    {
        al = malloc(sizeof(struct alias));
        strcpy(al->alias, alias);
        strcpy(al->cmd, cmd);
        al->next = NULL;
        return al;
    }
    if ((tmp = search(al, alias)) != NULL)
    { /* If the alias already exists,replace it with the new command */
        strcpy(tmp->cmd, cmd);
        return head;
    }
    while (al->next != NULL)
        al = al->next;
    al->next = malloc(sizeof(struct alias));
    strcpy(al->next->alias, alias);
    strcpy(al->next->cmd, cmd);
    al->next->next = NULL;
    return head;
}
alr search(alr al, char *alias)
{ /* Return a pointer to the alias/If not found,return NULL */
    if (al == NULL)
        return NULL;
    while (al->next != NULL)
    {
        if (!strcmp(alias, al->alias))
            return al;
        al = al->next;
    }
    if (!strcmp(alias, al->alias))
        return al;
    return NULL;
}

alr delal(alr al, char *alias)
{ /* Delete an alias */
    alr head = al, tmp;
    if (al == NULL)
    {
        printf("There are no alias assignments to delete\n");
        return NULL;
    }
    if (!strcmp(alias, al->alias))
    {
        head = al->next;
        free(al);
        return head;
    }
    while (al->next != NULL)
    {
        if (!strcmp(alias, al->next->alias))
        {
            tmp = al->next;
            al->next = al->next;
            free(tmp);
            return head;
        }
        al = al->next;
    }
    printf("Alias \"%s\" does not exist\n", alias);
    return head;
}
void dele(alr al)
{ /* Delete all aliases */
    alr prev;
    if (al == NULL)
        return;
    while (al->next != NULL)
    {
        prev = al;
        al = al->next;
        free(prev);
    }
    free(al);
}
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
        tokens = custom_tokenize(cp, tokens, &i, &last, &j, &flag);
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
        else
            tokens = custom_tokenize(cp, tokens, &i, &last, &j, &flag);
    }
    /* Free reserved memory and search for wild characters to replace */
    free(cp);
    free(copy);
    return wild(tokens);
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
    /* If there are remaining characters add them to a token */
    if (x - *last)
    {
        tokens[y] = malloc((x - *last) * sizeof(char) + 1);
        strncpy(tokens[y], cp + *last, x - *last);
        tokens[y][x - *last] = '\0';
    }
    *j = x;
    *i = y;
    return tokens;
}
int cd(char **tokens)
{ /* Support of cd command */
    char *cd_loc = tokens[1];
    if (cd_loc == NULL)
        cd_loc = getenv("HOME");
    int error = chdir(cd_loc);
    return error;
}

int redirection(char **tokens)
{
    int i = 0, fd, dsc;
    while (tokens[i] != NULL)
    {
        if (!strcmp(tokens[i], "<") || !strcmp(tokens[i], ">") || !strcmp(tokens[i], ">>"))
        {
            dsc = STDOUT_FILENO;
            char *file = tokens[i + 1];
            if (file == NULL)
            {
                printf("Missing file for redirection");
                return -1;
            }
            if (!strcmp(tokens[i], "<"))
            {
                dsc = STDIN_FILENO;
                fd = open(file, O_RDONLY);
            }
            else if (!strcmp(tokens[i], ">"))
                fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, 0644);
            else if (!strcmp(tokens[i], ">>"))
                fd = open(file, O_WRONLY | O_APPEND | O_CREAT, 0644);
            if (fd == -1)
            {
                perror("open");
                return -1;
            }
            close(dsc);
            if (dup2(fd, dsc) == -1)
            {
                perror("dup");
                return -1;
            }
            close(fd);
        }
        i++;
    }
    // return 0;
}
char **cleanup(char **tokens)
{
    char **args = malloc(TOKEN_NUM * sizeof(char *));

    for (int i = 0; i < TOKEN_NUM; i++)
        args[i] = NULL;
    int i = 0, x = 0;
    while (tokens[i] != NULL)
    {
        if (!strcmp(tokens[i], "<") || !strcmp(tokens[i], ">") || !strcmp(tokens[i], ">>"))
        {
            i += 2;
            continue;
        }
        if (!strcmp(tokens[i], ";") || !strcmp(tokens[i], "|") || !strcmp(tokens[i], "&"))
        {
            i++;
            continue;
        }
        if (tokens[i][0] == '\"')
        {
            args[x] = malloc(strlen(tokens[i]) * sizeof(char) - 1);
            strncpy(args[x], tokens[i] + 1, strlen(tokens[i]) - 2);
            args[x++][strlen(tokens[i++]) - 1] = '\0';
            continue;
        }
        args[x] = malloc(strlen(tokens[i]) * sizeof(char) + 1);
        strcpy(args[x++], tokens[i++]);
    }
    return args;
}
char ***frees(char ***tok, int total)
{
    if (tok != NULL)
    {
        for (int j = 0; j < total; j++)
        {
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
        }
        free(tok);
        tok = NULL;
    }
    return tok;
}

char **wild(char **tokens)
{
    glob_t globbuf;
    int flags = GLOB_NOCHECK | GLOB_TILDE, j = 0;
    char **args = malloc(TOKEN_NUM * sizeof(char *));
    for (int i = 0; i < TOKEN_NUM; i++)
        args[i] = NULL;
    for (int i = 0; tokens[i] != NULL; i++)
    {
        if (strchr(tokens[i], '*') != NULL || strchr(tokens[i], '?') != NULL)
        {

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
        {
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

int hs_al(char **tokens, ptr *hs, alr *al, char **str)
{
    alr alias;
    int num;
    if (!strcmp(tokens[0], "history"))
    {
        if (tokens[1] == NULL || (!strcmp(tokens[1], "|") || !strcmp(tokens[1], ";") || !strcmp(tokens[1], "&")))
            print((*hs));
        else if (sscanf(tokens[1], "%d", &num) == 1)
        {
            if (num > 0 && num <= (*hs)->count - 1)
            {
                ptr tmp = (*hs), prev = (*hs);
                tmp->count--;
                while (tmp->next != NULL)
                {
                    prev = tmp;
                    tmp = tmp->next;
                }
                prev->next = NULL;
                free(tmp);
                strcpy(*str, get_command(*hs, num));
                if ((*str)[strlen(*str) - 1] == '\n')
                    (*str)[strlen(*str) - 1] = '\0';
                int i = 2;
                while (tokens[i] != NULL)
                {
                    strcat(*str, " ");
                    strcat(*str, tokens[i++]);
                }
                strcat(*str, "\n");
                return num;
            }
            else
                printf("There is no %dth command in history...Try command \"history\" to see the command history\n", num);
            ptr tmp = (*hs), prev = (*hs);
            tmp->count--;
            if (tmp->next == NULL)
            {
                free(tmp);
                *hs = NULL;
            }
            else
            {
                while (tmp->next != NULL)
                {
                    prev = tmp;
                    tmp = tmp->next;
                }
                prev->next = NULL;
                free(tmp);
            }
        }
        else
            printf("Bad syntax of command \"history\".\nTry \"history\" or \"history <num>\" where num is in number in the range of 1..20\n");
    }
    else if (!strcmp(tokens[0], "createalias"))
    {
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
        else
            printf("Bad syntax of command \"createalias\".\nTry createalias <alias> \"command to alias\";\n");
    }
    else if (!strcmp(tokens[0], "destroyalias"))
    {
        if (tokens[2] != NULL)
            *al = delal(*al, tokens[1]);
        else
            printf("Bad syntax of command \"destroyalias\".\nTry destroyalias <alias>;\n");
    }
    else if ((alias = search(*al, tokens[0])) != NULL) // Check if command is an alias for an other command
    {
        strcpy(*str, alias->cmd);
        strcat(*str, " ");
        int i = 1;
        while (tokens[i] != NULL)
        {
            strcat(*str, tokens[i++]);
            strcat(*str, " ");
        }
        strcat(*str, "\n");
        return 0;
    }
    else
        return -2;
    return -1;
}

char ***separate(char ***tok, char **tokens, int *total)
{
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
    {

        tok = malloc(sizeof(char **));
        int j = 0, x = 0;
        tok[0] = malloc(sizeof(char *));
        for (int i = 0; i < TOKEN_NUM && tokens[i] != NULL; i++)
        {
            int flag = 1;
            if (!strcmp(tokens[i], "|") || !strcmp(tokens[i], ";") || !(flag = strcmp(tokens[i], "&")))
            {
                tok[j][x] = tokens[i];
                tok[j] = realloc(tok[j], (2 + x) * sizeof(char **));
                tok[j][++x] = NULL;
                if (tokens[i + 1] == NULL)
                    break;
                if (!flag && !strcmp(tokens[i + 1], ";"))
                {
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
            }
            tok[j][x] = tokens[i];
            tok[j] = realloc(tok[j], (2 + x) * sizeof(char **));
            tok[j][++x] = NULL;
        }
        *total = j + 1;
        return tok;
    }
    return NULL;
}