#ifndef WORLD_GUARD
#define WORLD_GUARD

#include <emscripten.h>
#include <stdio.h>

#include "bitfield.h"
#include "hashmap.h"
#include "list.h"
#include "meminst.h"
#include "tileset.h"

#define NULL_TILE -1
#define NULL_TILE_RENDER_DATA 0xFFFFFFFF

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
	Tileset* tileset;
} World;

extern EMSCRIPTEN_KEEPALIVE World* world_create(int chunk_size, Tileset* tileset);
extern EMSCRIPTEN_KEEPALIVE List32* world_get_undisplayed_chunks(World* world, int x, int y, int width, int height);
extern EMSCRIPTEN_KEEPALIVE uint32_t* world_get_chunk_render_data(World* world, Chunk* chunk);
extern EMSCRIPTEN_KEEPALIVE Chunk* world_create_chunk(World* world, int x, int y);
extern EMSCRIPTEN_KEEPALIVE Chunk* world_get_chunk(World* world, int x, int y);
extern EMSCRIPTEN_KEEPALIVE int world_set(World* world, int x, int y, int tile);
extern EMSCRIPTEN_KEEPALIVE int world_get(World* world, int x, int y);
extern EMSCRIPTEN_KEEPALIVE void world_free(World* world);

#endif