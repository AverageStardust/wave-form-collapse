#include "world.h"

void free_chunk(Chunk* chunk) {
	free(chunk->tiles);
	free(chunk);
}

void world_free(World* world) {
	hashmap_free(world->chunks, free_chunk);
	free(world);
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
	Chunk* chunk = malloc(sizeof(Chunk));

	if (chunk == NULL) {
		fprintf(stderr, "Failed to allocate memory: world_create_chunk()");
		exit(1);
	}

	chunk->tiles = malloc(world->chunk_size * world->chunk_size * sizeof(int));

	if (chunk->tiles == NULL) {
		fprintf(stderr, "Failed to allocate memory: world_create_chunk()");
		exit(1);
	}

	for (int i = 0; i < world->chunk_size * world->chunk_size; i++) {
		chunk->tiles[i] = NULL_TILE;
	}

	chunk->x = x;
	chunk->y = y;

	chunk->is_displayed = 0;

	hashmap_set(world->chunks, hashkey_from_pair(x, y), chunk);

	return chunk;
}

IntList* world_get_undisplayed_chunks(World* world, int x, int y, int width, int height) {
	IntList* list = intlist_create(4);

	for (int u = x; u < x + width; u++) {
		for (int v = y; v < y + height; v++) {
			Chunk* chunk = world_get_chunk(world, u, v);
			if (chunk != NULL && !chunk->is_displayed)
				intlist_push(list, chunk);
		}
	}

	return list;
}

World* world_create(uint32_t chunk_size) {
	World* world = malloc(sizeof(World));

	if (world == NULL) {
		fprintf(stderr, "Failed to allocate memory: world_create()");
		exit(1);
	}

	world->chunks = hashmap_create(256);

	if (world->chunks == NULL) {
		fprintf(stderr, "Failed to allocate memory: world_create()");
		exit(1);
	}

	if (chunk_size != 0 || (chunk_size & (chunk_size - 1))) {
		fprintf(stderr, "Chunk size must be a power of two: world_create()");
		exit(1);
	}

	world->chunk_bits = 31 - __builtin_clz(chunk_size);
	world->chunk_size = chunk_size;
	world->chunk_mask = chunk_size - 1;

	return world;
}