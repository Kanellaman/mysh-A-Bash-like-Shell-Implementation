#include "../Include/Interface.h"
/* History handling */
ptr append(ptr hs, char *str)
{ /* Add a command to the myhistory */
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
    if (!strcmp(str, hs->command)) // If you try to insert the same command as the last one in myhistory, don't
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
{ /* Print the myhistory */
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
void del(ptr hs)
{ /* Free the memory that myhistory uses */
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
char *get_command(ptr hs, int num)
{ /* Returns the i-th command of the myhistory */
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

/* Aliases handling */
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
