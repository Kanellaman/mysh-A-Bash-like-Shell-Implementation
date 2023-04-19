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
    if (hs == NULL)
        return;
    while (hs->next != NULL)
    {
        ptr prev = hs;
        hs = hs->next;
        free(prev);
    }
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


int valid(char *str)
{
    return 1;
}

char **tokenize(char *copy)
{
    int i = 0, last = 0, j = 0;
    char *cp, **tokens = malloc(20 * sizeof(char *));
    cp = strtok(copy, " \n");
    if (cp == NULL)
        return NULL;
    tokens = custom_tokenize(cp, tokens, &i, &last, &j);
    tokens[i] = malloc((j - last) * sizeof(char) + 1);
    strncpy(tokens[i], cp + last, j - last);
    tokens[i][j - last] = '\0';
    if (!strlen(tokens[i])) /* If the last token is a character <,>,|... */
    {
        free(tokens[i]);
        tokens[i--] = NULL;
    }
    while (cp != NULL)
    {
        last = 0;
        i++;
        cp = strtok(NULL, " \n");
        if (cp != NULL)
        {
            tokens = custom_tokenize(cp, tokens, &i, &last, &j);
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
char **custom_tokenize(char *cp, char **tokens, int *i, int *last, int *j)
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
            if (x != 0)
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
            fd = open(file, O_WRONLY | O_TRUNC | O_CREAT);
        }
        else
        {
            dsc = STDOUT_FILENO;
            fd = open(file, O_WRONLY | O_APPEND | O_CREAT);
        }
        if (fd == -1)
        {
            printf("%ld %d ", strlen(file), dsc);
            printf("OXI? %s", file);
            // frees(copy, str, tokens, "open");
            exit(5);
        }
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
    free(tokens);
    exit(EXIT_FAILURE);
}