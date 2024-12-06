#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "hashtable.h"
#include "prime.h"

static int HT_PRIME_1 = 151;
static int HT_PRIME_2 = 163;
static item HT_DELETED_ITEM = {NULL, NULL};
static int HT_INITIAL_BASE_SIZE = 53;

static item* new_item(const char* k, const char* v) {
    item* i = malloc(sizeof(item));
    i->key = strdup(k);
    i->value = strdup(v);
    return i;
}

// Size defines how many items we can store
hashtable* new_hashtable() {
    return new_sized_hashtable(HT_INITIAL_BASE_SIZE);
}

static hashtable* new_sized_hashtable(const int base_size) {
    hashtable* ht = malloc(sizeof(hashtable));
    ht->base_size = base_size;
    ht->size = next_prime(ht->base_size);
    ht->count = 0;
    ht->items = calloc((size_t)ht->size, sizeof(item*));
    return ht;
}

static void resize(hashtable* ht, const int base_size) {
    if (base_size < HT_INITIAL_BASE_SIZE) {
        return;
    }
    hashtable* new_ht = new_sized_hashtable(base_size);
    for (int i = 0; i < ht->size; i++) {
        item* it = ht->items[i];
        if (it != NULL && it != &HT_DELETED_ITEM) {
            insert(new_ht, it->key, it->value);
        }
    }
    ht->base_size = new_ht->base_size;
    ht->count = new_ht->count;
    // deleting new_ht
    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;
    item** tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;
    del_table(new_ht);
}

static void resize_up(hashtable* ht) {
    const int new_size = ht->base_size * 2;
    resize(ht, new_size);
}

static void resize_down(hashtable* ht) {
    const int new_size = ht->base_size / 2;
    resize(ht, new_size);
}

static void del_item(item* i) {
    free(i->key);
    free(i->value);
    free(i);
}

void del_table(hashtable* ht) {
    for (int i = 0; i < ht->size; i++) {
        item* it = ht->items[i];
        if (it != NULL) {
            del_item(it);
        }
    }
    free(ht->items);
    free(ht);
}

// -----------------------------------------------------

// Hash generator
static int hasher(const char* s, const int a, const int m) {
    long hash = 0;
    const int len_s = strlen(s);
    for (int i = 0; i < len_s; i++) {
        hash += (long)pow(a, len_s - (i+1)) * s[i];
        hash = hash % m;
    }
    return (int)hash;
}

// Handling Collisions with hashes
static int get_hash(const char*s, const int buckets, const int attempt) {
    const int a = hasher(s, HT_PRIME_1, buckets);
    const int b = hasher(s, HT_PRIME_2, buckets);
    return (a + (attempt * (b + 1))) % buckets;
}

// -----------------------------------------------------

void insert(hashtable* ht, const char* key, const char* value) {
    // Check load on hashtable, if above 70%, resize up
    const int load = ht->count * 100 / ht->size;
    if (load > 70) {
        resize_up(ht);
    }
    item* it = new_item(key, value);
    int index = get_hash(it->key, ht->size, 0);
    item* cur_it = ht->items[index];
    int i = 1;
    while (cur_it != NULL) {
        if (cur_it != &HT_DELETED_ITEM) {
            if (strcmp(cur_it->key, key) == 0) {
                del_item(cur_it);
                ht->items[index] = it;
                return;
            }
        }
        index = get_hash(it->key, ht->size, i);
        cur_it = ht->items[index];
        i++;
    }
    ht->items[index] = it;
    ht->count++;
}

char* search(hashtable* ht, const char* key) {
    int index = get_hash(key, ht->size, 0);
    item* it = ht->items[index];
    int i = 1;
    while (it != NULL) {
        if (it != &HT_DELETED_ITEM) {
            if (strcmp(it->key, key) == 0) {
                return it->value;
            }
        }
        index = get_hash(key, ht->size, i);
        it = ht->items[index];
        i++;
    }
    return NULL;
}

void delete(hashtable* ht, const char* key) {
    // Check load on hashtable, if below 10%, resize down
    const int load = ht->count * 100 / ht->size;
    if (load < 10) {
        resize_down(ht);
    }
    int index = get_hash(key, ht->size, 0);
    item* it = ht->items[index];
    int i = 1;
    while (it != NULL) {
        if (it != &HT_DELETED_ITEM) {
            if (strcmp(it->key, key) == 0) {
                del_item(it);
                ht->items[index] = &HT_DELETED_ITEM;
            }
        }
        index = get_hash(key, ht->size, i);
        it = ht->items[index];
        i++;
    }
    ht->count--;
}
