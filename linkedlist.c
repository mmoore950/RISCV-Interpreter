#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

typedef struct linkedlist_node linkedlist_node_t;

struct linkedlist
{
    struct linkedlist_node *first;
    unsigned int length;
};

struct linkedlist_node
{

    int key;   // Key (memory address)
    int value; // Value (at memory address)
    linkedlist_node_t *next;
};
typedef struct linkedlist_node linkedlist_node_t;

linkedlist_t *ll_init()
{

    linkedlist_t *list = malloc(sizeof(linkedlist_t));

    list->first = NULL;
    list->length = 0;
    return list;
}

void ll_free(linkedlist_t *list)
{

    linkedlist_node_t *current = list->first;
    linkedlist_node_t *next;
    while (current != NULL)
    {
        next = current->next;
        free(current);
        current = next;
    }
    free(list);
}

void ll_add(linkedlist_t *list, int key, int value)
{

    linkedlist_node_t *current = list->first;
    while (current != NULL)
    {
        if (current->key == key)
        {
            current->value = value;
            return;
        }
        current = current->next;
    }
    linkedlist_node_t *new = malloc(sizeof(linkedlist_node_t));
    new->key = key;
    new->value = value;
    new->next = list->first;
    list->first = new;
    list->length++;
}

int ll_get(linkedlist_t *list, int key)
{

    linkedlist_node_t *current = list->first;
    while (current != NULL)
    {
        if (current->key == key)
        {
            return current->value;
        }
        current = current->next;
    }
    return 0;
}

int ll_size(linkedlist_t *list)
{

    return list->length;
}
