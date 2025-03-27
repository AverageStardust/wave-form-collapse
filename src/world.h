#ifndef WORLD_GUARD
#define WORLD_GUARD

#include <emscripten.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitfield.h"
#include "hashmap.h"
#include "list.h"

#define NULL_TILE -1

typedef struct {
	int x;
	int y;
	int is_displayed;
	int generation_stage;
	int* tiles;
} Chunk;

typedef struct {
	int chunk_size;
	int chunk_bits;
	int chunk_mask;
	Hashmap* chunks;
} World;

extern EMSCRIPTEN_KEEPALIVE World* world_create(uint32_t chunk_size);
extern EMSCRIPTEN_KEEPALIVE List64* world_get_undisplayed_chunks(World* world, int x, int y, int width, int height);
extern EMSCRIPTEN_KEEPALIVE Chunk* world_create_chunk(World* world, int x, int y);
extern EMSCRIPTEN_KEEPALIVE Chunk* world_get_chunk(World* world, int x, int y);
extern EMSCRIPTEN_KEEPALIVE int world_set(World* world, int x, int y, int tile);
extern EMSCRIPTEN_KEEPALIVE int world_get(World* world, int x, int y);
extern EMSCRIPTEN_KEEPALIVE void world_free(World* world);

#endif