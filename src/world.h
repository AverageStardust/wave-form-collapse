#ifndef WORLD_GUARD
#define WORLD_GUARD

#include <stdio.h>
#include <stdlib.h>

#include "bitfield.h"
#include "hashmap.h"

#define CHUNK_BITS 4
#define CHUNK_SIZE (1 << CHUNK_BITS)
#define CHUNK_MASK (CHUNK_SIZE - 1)
#define NULL_TILE -1

typedef struct {
	int* tiles;
} Chunk;

typedef struct {
	Hashmap* chunks;
} World;

World* world_create();
int world_set(World* world, int x, int y, int tile);
int world_get(World* world, int x, int y);
Chunk* world_create_chunk(World* world, int x, int y);

#endif