#ifndef HASHMAP_GUARD
#define HASHMAP_GUARD

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "meminst.h"

typedef struct HashmapNode {
	struct HashmapNode* next;
	void* value;
	uint64_t key;
} HashmapNode;

typedef struct {
	int size;
	int collisions;
	HashmapNode** nodes;
} Hashmap;

#define hashkey_from_pair(x, y) ((uint64_t)(unsigned int)x + ((uint64_t)(unsigned int)y << 32))
#define x_from_hashkey(key) ((unsigned int)(key & 0xFFFFFFFF))
#define y_from_hashkey(key) ((unsigned int)(key >> 32))

Hashmap* hashmap_create(int inital_size);
void* hashmap_set(Hashmap* hashmap, uint64_t key, void* value);
void* hashmap_get(Hashmap* hashmap, uint64_t key);
int hashmap_has(Hashmap* hashmap, uint64_t key);
void* hashmap_delete(Hashmap* hashmap, uint64_t key);
void hashmap_map(Hashmap* hashmap, void* (*map_func)(uint64_t key, void* value));
void hashmap_clear(Hashmap* hashmap, int new_size);
void hashmap_free(Hashmap* hashmap, void (*free_value)(void* value));

#endif