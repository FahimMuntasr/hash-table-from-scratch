#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "prime.h"
#define HT_INITIAL_BASE_SIZE 53
#define HT_PRIME_1 151
#define HT_PRIME_2 163

static ht_item HT_DELETED_ITEM = {NULL, NULL};

static void ht_resize_up(ht_hash_table* ht);
static void ht_resize_down(ht_hash_table* ht);
static void ht_resize(ht_hash_table* ht, const int base_size);

static void* xmalloc(size_t size) {
    void* p = malloc(size);

    if (p == NULL) {
        exit(EXIT_FAILURE);
    }

    return p;
}

static void* xcalloc(size_t count, size_t size) {
    void* p = calloc(count, size);

    if (p == NULL) {
        exit(EXIT_FAILURE);
    }

    return p;
}

static char* ht_strdup(const char* str) {
    size_t len = strlen(str) + 1;

    char* copy = malloc(len);

    if (copy == NULL) {
        return NULL;
    }

    memcpy(copy, str, len);

    return copy;
}

static ht_item* ht_new_item(const char* k, const char* v) {
    ht_item* item = malloc(sizeof(ht_item));

    if (item == NULL) {
        return NULL;
    }

    item->key = ht_strdup(k);
    item->value = ht_strdup(v);

    if (item->key == NULL || item->value == NULL) {
        free(item->key);
        free(item->value);
        free(item);
        return NULL;
    }

    return item;
}

static void ht_del_item(ht_item* item){
    free(item->key);
    free(item->value);
    free(item);
}

void ht_del_hash_table(ht_hash_table* table){
    for(int i=0; i<table->size; i++){
        ht_item* item = table->items[i];
        if(item != NULL && item != &HT_DELETED_ITEM){
            ht_del_item(item);
        }
    }
    free(table->items);
    free(table);
}

static int ht_hash(const char* s, const int a, const int m){
  long hash = 0;
  const int len_s = strlen(s);

  for(int i=0;i<len_s;i++){
    hash += (long)pow(a, len_s - (i+1))*s[i];
    hash %= m;
  }
  return (int)hash;
}

static int ht_get_hash(const char* s, const int bucket_num, const int attempt){
  const int hash_a = ht_hash(s, HT_PRIME_1, bucket_num);
  const int hash_b = ht_hash(s, HT_PRIME_2, bucket_num);
  return (hash_a + (attempt * (hash_b+1))) % bucket_num;
}

void ht_insert(ht_hash_table* ht, const char* key, const char* value){
  const int load = ht->count * 100 / ht->size;
  if (load > 70) {
    ht_resize_up(ht);
  }
  ht_item* item = ht_new_item(key, value);

  int index = ht_get_hash(item->key, ht->size, 0); 
  int i = 1;
  int deleted_index = -1;

  while (ht->items[index] != NULL) {

    if (ht->items[index] == &HT_DELETED_ITEM) {
      if (deleted_index == -1) {
        deleted_index = index;
      }
    }
    else if (strcmp(ht->items[index]->key, key) == 0) {
      ht_del_item(ht->items[index]);
      ht->items[index] = item;
      return;
    }

    index = ht_get_hash(key, ht->size, i);
    i++;
  }

  if (deleted_index != -1) {
      index = deleted_index;
  }

  ht->items[index] = item;
  ht->count++;
}

char* ht_search(ht_hash_table* ht, const char* key){
  int index = ht_get_hash(key, ht->size, 0);
  ht_item* item = ht->items[index];
  
  int i = 1;
  
  while(item!=NULL){
    if(item!=&HT_DELETED_ITEM){
      if(strcmp(item->key, key)==0){
        return item->value;
      }
    }
    index = ht_get_hash(key, ht->size, i);
    item = ht->items[index];
    i++;
  }
  return NULL;
}

void ht_delete(ht_hash_table* ht, const char* key) {
    int index = ht_get_hash(key, ht->size, 0);
    ht_item* item = ht->items[index];

    int i = 1;

    while (item != NULL) {

        if (item != &HT_DELETED_ITEM) {

            if (strcmp(item->key, key) == 0) {
                ht_del_item(item);
                ht->items[index] = &HT_DELETED_ITEM;
                ht->count--;

                if (ht->count * 100 / ht->size < 10) {
                    ht_resize_down(ht);
                }

                return;
            }
        }

        index = ht_get_hash(key, ht->size, i);
        item = ht->items[index];
        i++;
    }
}


static ht_hash_table* ht_new_sized(const int base_size) {
    ht_hash_table* ht = xmalloc(sizeof(ht_hash_table));
    ht->base_size = base_size;

    ht->size = next_prime(ht->base_size);

    ht->count = 0;
    ht->items = xcalloc((size_t)ht->size, sizeof(ht_item*));
    return ht;
}


ht_hash_table* ht_new() {
    return ht_new_sized(HT_INITIAL_BASE_SIZE);
}

static void ht_resize(ht_hash_table* ht, const int base_size) {
    if (base_size < HT_INITIAL_BASE_SIZE) {
        return;
    }
    ht_hash_table* new_ht = ht_new_sized(base_size);
    for (int i = 0; i < ht->size; i++) {
        ht_item* item = ht->items[i];
        if (item != NULL && item != &HT_DELETED_ITEM) {
            ht_insert(new_ht, item->key, item->value);
        }
    }

    ht->base_size = new_ht->base_size;
    ht->count = new_ht->count;

    // To delete new_ht, we give it ht's size and items 
    const int tmp_size = ht->size;
    ht->size = new_ht->size;
    new_ht->size = tmp_size;

    ht_item** tmp_items = ht->items;
    ht->items = new_ht->items;
    new_ht->items = tmp_items;

    ht_del_hash_table(new_ht);
}

static void ht_resize_up(ht_hash_table* ht) {
    const int new_size = ht->base_size * 2;
    ht_resize(ht, new_size);
}


static void ht_resize_down(ht_hash_table* ht) {
    const int new_size = ht->base_size / 2;
    ht_resize(ht, new_size);
}

