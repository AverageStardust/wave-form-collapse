#ifndef WORLD_GUARD
#define WORLD_GUARD

#include <stdio.h>
#include <stdlib.h>

#include "bitfield.h"
#include "hashmap.h"

#define NULL_TILE -1

typedef struct {
	int* tiles;
	int is_displayed;
} Chunk;

typedef struct {
	Hashmap* chunks;
	int chunk_size;
	int chunk_bits;
	int chunk_mask;
} World;

World* world_create(uint32_t chunk_size);
Chunk* world_create_chunk(World* world, int x, int y);
Chunk* world_get_chunk(World* world, int x, int y);
int world_set(World* world, int x, int y, int tile);
int world_get(World* world, int x, int y);

#endif