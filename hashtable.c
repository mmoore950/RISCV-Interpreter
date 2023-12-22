#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "linkedlist.h"
#include "hashtable.h"

struct hashtable
{

    linkedlist_t **buckets;
    unsigned int numBuckets;
    unsigned int length;
};


static int hash(int key, int max_range)
{
    key = (key > 0) ? key : -key;
    return key % max_range;
}

hashtable_t *ht_init(int num_buckets)
{

    hashtable_t *table = malloc(sizeof(hashtable_t));
    table->buckets = malloc(sizeof(linkedlist_t *) * num_buckets);
    for (int i = 0; i < num_buckets; i++)
    {
        table->buckets[i] = ll_init();
    }
    table->numBuckets = num_buckets;
    table->length = 0;
    return table;
}

void ht_free(hashtable_t *table)
{

    for (int i = 0; i < table->numBuckets; i++)
    {
        ll_free(table->buckets[i]);
    }
    free(table->buckets);
    free(table);
}

void ht_add(hashtable_t *table, int key, int value)
{

    int bucketIndex = hash(key, table->numBuckets);
    if (ll_get(table->buckets[bucketIndex], key) == 0)
    {
        table->length++;
    }
    ll_add(table->buckets[bucketIndex], key, value);
}

int ht_get(hashtable_t *table, int key)
{

    int bucketIndex = hash(key, table->numBuckets);
    return ll_get(table->buckets[bucketIndex], key);
}

int ht_size(hashtable_t *table)
{

    return table->length;
}
