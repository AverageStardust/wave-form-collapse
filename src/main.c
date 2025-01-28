#include "main.h"

World* world;

int main() {
	srand(time(0));

	world = world_create();
	world_create_chunk(world, 0, 0);
	world_set(world, 0, 0, 0);
	world_set(world, 1, 0, 1);
	world_set(world, 2, 0, 2);

	return 0;
}

extern EMSCRIPTEN_KEEPALIVE int get_chunk_size() {
	return CHUNK_SIZE;
}

extern EMSCRIPTEN_KEEPALIVE IntList* get_undisplayed_chunks(int x, int y, int width, int height) {
	IntList* list = intlist_create(2);

	for (int u = x; u < x + width; u++) {
		for (int v = y; v < y + height; v++) {
			Chunk* chunk = world_get_chunk(world, u, v);
			if (chunk != NULL && !chunk->is_displayed) {
				intlist_push(list, u);
				intlist_push(list, v);
			}
		}
	}

	return list;
}

extern EMSCRIPTEN_KEEPALIVE int* get_chunk_tiles(int x, int y, int mark_displayed) {
	Chunk* chunk = world_get_chunk(world, x, y);
	if (chunk == NULL) return NULL;

	if (mark_displayed)
		chunk->is_displayed = 1;

	return chunk->tiles;
}

// create chunk, world
// create renderer
// create entropy heap
// create generator