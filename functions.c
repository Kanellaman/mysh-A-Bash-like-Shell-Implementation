#include "Interface.h"
char *get_command(ptr hs, int num)
{
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
{
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
{
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
    {
        if (!strcmp(str, hs->command))
            return head;
        hs = hs->next;
    }
    if (!strcmp(str, hs->command))
        return head;

    head->count = head->count + 1;
    if (head->count == 20)
    {
        hs = head->next;
        hs->count = 19;
        free(head);
        head = hs;
    }
    while (hs->next != NULL)
        hs = hs->next;
    hs->next = malloc(sizeof(struct history));
    strcpy(hs->next->command, str);
    hs->next->count = 0;
    hs->next->next = NULL;
    return head;
}
void print(ptr hs)
{
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
{
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
    {
        printf("Alias \"%s\" is already assigned to command \"%s\"\n", alias, tmp->cmd);
        strcpy(tmp->cmd, cmd);
        return head;
    }
    while (al->next != NULL)
    {
        al = al->next;
    }
    al->next = malloc(sizeof(struct alias));
    strcpy(al->next->alias, alias);
    strcpy(al->next->cmd, cmd);
    al->next->next = NULL;
    return head;
}
alr search(alr al, char *alias)
{
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
{
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
{
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
char **tokenize(char *copy)
{
    int i = 0, last = 0, j = 0, count = 0;
    char *cp, **tokens = malloc(TOKEN_NUM * sizeof(char *));
    bool flag = false;
    for (int i = 0; i < strlen(copy); i++)
        if (copy[i] == '\"')
            count++;
    if (count % 2 != 0)
    {
        printf("There is \" that never closes\nSyntax error\n");
        exit(0);
    }

    cp = strtok(copy, " \n");
    if (cp == NULL)
        return NULL;
    else if (cp[0] == '"')
    {
        tokens[i] = malloc(LINE_SIZE * sizeof(char) + 1);
        strcpy(tokens[i], cp);

        if (cp[strlen(cp) - 1] != '"')
        {
            cp = strtok(NULL, "\"");
            if (cp != NULL)
            {
                if (cp[0] == '\n')
                    cp[0] = ' ';
                else
                    strcat(tokens[i], " ");

                strcat(tokens[i], cp);
                strcat(tokens[i], "\"");
            }
        }
        if (tokens[i][strlen(tokens[i]) - 1] != '\"')
        {
            char ch = tokens[i][strlen(tokens[i]) - 1];
            tokens[i][strlen(tokens[i]) - 1] = '\0';
            tokens[++i] = malloc(sizeof(char) + 1);
            sprintf(tokens[i], "%c", ch);
        }
    }
    else
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
    while (cp != NULL)
    {
        last = 0;
        i++;
        cp = strtok(NULL, " \n");
        if (cp == NULL)
        {
            break;
        }
        else if (cp[0] == '"')
        {
            tokens[i] = malloc(LINE_SIZE * sizeof(char) + 1);
            strcpy(tokens[i], cp);
            if (cp[strlen(cp) - 1] != '"')
            {
                cp = strtok(NULL, "\"");
                if (cp != NULL)
                {
                    if (cp[0] == '\n')
                        cp[0] = ' ';
                    else
                        strcat(tokens[i], " ");

                    strcat(tokens[i], cp);
                    strcat(tokens[i], "\"");
                }
            }
            if (tokens[i][strlen(tokens[i]) - 1] != '\"')
            {
                char ch = tokens[i][strlen(tokens[i]) - 1];
                tokens[i][strlen(tokens[i]) - 1] = '\0';
                tokens[++i] = malloc(sizeof(char) + 1);
                sprintf(tokens[i], "%c", ch);
            }
        }
        else
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
    free(cp);
    free(copy);
    return tokens;
}
char **custom_tokenize(char *cp, char **tokens, int *i, int *last, int *j, bool *flag)
{
    int x, y = *i;
    for (x = 0; x < strlen(cp); x++)
    {
        if (cp[x] == '>' && x + 1 < strlen(cp) && cp[x + 1] == '>')
        {
            char *s = ">>";

            if (x != 0)
            {
                tokens[y] = malloc((x - *last) * sizeof(char) + 1);
                strncpy(tokens[y++], cp + *last, x - *last);
                tokens[y - 1][x - *last] = '\0';
            }
            tokens[y] = malloc(sizeof(char) + 1);
            strcpy(tokens[y++], s);
            x++;
            *last = x + 1;
        }
        else if (cp[x] == '<' || cp[x] == '>' || cp[x] == '|' || cp[x] == '&' || cp[x] == ';')
        {
            char s[2];
            s[0] = cp[x];
            s[1] = '\0';
            if (x != 0 && *last != x)
            {
                tokens[y] = malloc((x - *last) * sizeof(char) + 1);
                strncpy(tokens[y++], cp + *last, x - *last);
                tokens[y - 1][x - *last] = '\0';
            }
            tokens[y] = malloc(sizeof(char) + 1);
            strcpy(tokens[y++], s);
            *last = x + 1;
        }
    }
    *j = x;
    *i = y;
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
            fd = open(file, O_WRONLY | O_TRUNC | O_CREAT, 0644);
        }
        else
        {
            dsc = STDOUT_FILENO;
            fd = open(file, O_WRONLY | O_APPEND | O_CREAT, 0644);
        }
        if (fd == -1)
        {
            perror("open");
            exit(0);
        }
        close(dsc);
        if (dup2(fd, dsc) == -1)
            exit(0);
        close(fd);
    }
    return 0;
}

void frees(char *str, char *copy, char **tokens, char *s)
{
    perror(s);
    free(str);
    free(tokens);
    exit(EXIT_FAILURE);
}