#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4097

typedef struct Node
{
    char *txt;
    struct Node *next;
} Node;

void print_list(Node *head)
{
    if (!head)
        return;
    Node *cur = head;
    printf("\nPrinting list:\n");
    while (cur)
    {
        printf("%s\n", cur->txt);
        cur = cur->next;
    }
}

void free_list(Node *head)
{
    if (!head)
        return;
    Node *cur = head;
    while (cur)
    {
        Node *tmp = cur;
        cur = cur->next;
        free(tmp->txt);
        free(tmp);
    }
}

int main(void)
{
    Node *head = NULL;
    Node *tail = NULL;
    char buffer[BUFFER_SIZE];

    printf("Введите строку. Для завершения введите .\n");
    while (1)
    {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
        {
            break;
        }

        if (buffer[0] == '.')
        {
            break;
        }

        size_t len = sizeof(buffer);

        Node *new_node = (Node *)malloc(sizeof(Node));
        new_node->next = NULL;
        new_node->txt = (char *)malloc(len + 1);
        if (!new_node->txt)
        {
            perror("Error allocating memory for new_node->txt");
            free(new_node);
            return 1;
        }

        strcpy(new_node->txt, buffer);

        if (!head)
        {
            head = new_node;
            tail = new_node;
        }
        else
        {
            tail->next = new_node;
            tail = new_node;
        }
    }

    print_list(head);
    free_list(head);
    return 0;
}