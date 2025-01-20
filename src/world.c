#include "world.h"

int world_get(World* world, int x, int y) {
	Chunk* chunk = world_get_chunk(world, x >> CHUNK_BITS, y >> CHUNK_BITS);
	if (chunk == NULL) return NULL_TILE;

	return chunk->tiles[(x & CHUNK_MASK) + (y & CHUNK_MASK) * CHUNK_SIZE];
}

int world_set(World* world, int x, int y, int tile) {
	Chunk* chunk = world_get_chunk(world, x >> CHUNK_BITS, y >> CHUNK_BITS);
	if (chunk == NULL) return 0;

	chunk->tiles[(x & CHUNK_MASK) + (y & CHUNK_MASK) * CHUNK_SIZE] = tile;
	chunk->is_displayed = 0;

	return 1;
}

Chunk* world_get_chunk(World* world, int x, int y) {
	uint64_t key = (uint64_t)(unsigned int)x + ((uint64_t)(unsigned int)y << 32);
	return hashmap_get(world->chunks, key);
}

Chunk* world_create_chunk(World* world, int x, int y) {
	uint64_t key = (uint64_t)x + ((uint64_t)y << 32);

	Chunk* chunk = malloc(sizeof(Chunk));

	if (chunk == NULL) {
		fprintf(stderr, "Failed to allocate memory: world_create_chunk()");
		exit(1);
	}

	chunk->tiles = malloc(CHUNK_SIZE * CHUNK_SIZE * sizeof(int));

	if (chunk->tiles == NULL) {
		fprintf(stderr, "Failed to allocate memory: world_create_chunk()");
		exit(1);
	}

	for (int i = 0; i < CHUNK_SIZE * CHUNK_SIZE; i++) {
		chunk->tiles[i] = NULL_TILE;
	}

	chunk->is_displayed = 0;

	hashmap_set(world->chunks, key, chunk);

	return chunk;
}

World* world_create() {
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

	return world;
}