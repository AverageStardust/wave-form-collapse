#include "world.h"

void free_chunk(void* chunk) {
	free_inst(((Chunk*)chunk)->tiles);
	free_inst((Chunk*)chunk);
}

void world_free(World* world) {
	hashmap_free(world->chunks, free_chunk);
	free_inst(world);
}

int world_get(World* world, int x, int y) {
	Chunk* chunk = world_get_chunk(world, x >> world->chunk_bits, y >> world->chunk_bits);
	if (chunk == NULL) return NULL_TILE;

	return chunk->tiles[(x & world->chunk_mask) + (y & world->chunk_mask) * world->chunk_size];
}

int world_set(World* world, int x, int y, int tile) {
	Chunk* chunk = world_get_chunk(world, x >> world->chunk_bits, y >> world->chunk_bits);
	if (chunk == NULL) return 0;

	chunk->tiles[(x & world->chunk_mask) + (y & world->chunk_mask) * world->chunk_size] = tile;
	chunk->is_displayed = 0;

	return 1;
}

Chunk* world_get_chunk(World* world, int x, int y) {
	return hashmap_get(world->chunks, hashkey_from_pair(x, y));
}

Chunk* world_create_chunk(World* world, int x, int y) {
	Chunk* chunk = malloc_inst(sizeof(Chunk));

	if (chunk == NULL) {
		fprintf(stderr, "Failed to allocate memory: world_create_chunk()\n");
		exit(1);
	}

	chunk->tiles = malloc_inst(world->chunk_size * world->chunk_size * sizeof(int));

	if (chunk->tiles == NULL) {
		fprintf(stderr, "Failed to allocate memory: world_create_chunk()\n");
		exit(1);
	}

	for (int i = 0; i < world->chunk_size * world->chunk_size; i++) {
		chunk->tiles[i] = NULL_TILE;
	}

	chunk->x = x;
	chunk->y = y;

	chunk->is_displayed = 0;
	chunk->generation_stage = 0;

	hashmap_set(world->chunks, hashkey_from_pair(x, y), chunk);

	return chunk;
}

uint32_t* world_get_chunk_render_data(World* world, Chunk* chunk) {
	uint32_t* render_data = malloc_inst(world->chunk_size * world->chunk_size * sizeof(uint32_t));

	for (int i = 0; i < world->chunk_size * world->chunk_size; i++) {
		if (chunk->tiles[i] == -1) {
			render_data[i] = -1;
		} else {
			render_data[i] = world->tileset->render_data_table[chunk->tiles[i]];
		}
	}

	return render_data;
}

List32* world_get_undisplayed_chunks(World* world, int x, int y, int width, int height) {
	List32* list = list32_create(4);

	for (int u = x; u < x + width; u++) {
		for (int v = y; v < y + height; v++) {
			Chunk* chunk = world_get_chunk(world, u, v);
			if (chunk != NULL && !chunk->is_displayed)
				list32_push(list, (uint32_t)chunk);
		}
	}

	return list;
}

World* world_create(int chunk_size, Tileset* tileset) {
	World* world = malloc_inst(sizeof(World));

	if (world == NULL) {
		fprintf(stderr, "Failed to allocate memory: world_create()\n");
		exit(1);
	}

	if (chunk_size == 0 || (chunk_size & (chunk_size - 1))) {
		fprintf(stderr, "Chunk size must be a power of two: world_create()\n");
		exit(1);
	}

	world->chunk_bits = 31 - __builtin_clz((uint32_t)chunk_size);
	world->chunk_size = chunk_size;
	world->chunk_mask = chunk_size - 1;

	world->chunks = hashmap_create(256);
	world->tileset = tileset;

	return world;
}