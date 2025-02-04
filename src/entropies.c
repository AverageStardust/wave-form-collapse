#include "entropies.h"

// ported from JS
// holds entropy data for Superposition
// allow quick to:
//   - update entropy by coordinate with a 2d array
//   - to find minimum entropy with a heap
// these two data structures are updated in sync

void entropies_free(Entropies* entropies) {
	free(entropies->tiles);
	free(entropies->tile_nodes);

	free(entropies->keys);
	free(entropies->values);

	free(entropies);
}

void entropies_heap_swim(Entropies* entropies, GenerationHeapNode node) {
	GenerationTile key = entropies->keys[node];
	Entropy value = entropies->values[node];

	while (node > 1) {
		GenerationHeapNode parent = node >> 1;

		if (value >= entropies->values[parent]) break;

		GenerationTile parent_key = entropies->keys[parent];
		entropies->keys[node] = parent_key;
		entropies->tile_nodes[parent_key] = node;
		entropies->values[node] = entropies->values[parent];

		node = parent;
	}

	entropies->keys[node] = key;
	entropies->tile_nodes[key] = node;
	entropies->values[node] = value;
}

void entropies_heap_sink(Entropies* entropies, GenerationHeapNode node) {
	GenerationTile key = entropies->keys[node];
	Entropy value = entropies->values[node];
	GenerationHeapNode half_size = entropies->heap_size >> 1;

	while (node <= half_size) {
		GenerationHeapNode left = node << 1;
		GenerationHeapNode right = left + 1;

		Entropy left_value = entropies->values[left];

		GenerationHeapNode child;
		Entropy child_value;

		if (right <= entropies->heap_size) {
			Entropy right_value = entropies->values[right];

			if (left_value < right_value) {
				child = left;
				child_value = left_value;
			} else {
				child = right;
				child_value = right_value;
			}
		} else {
			child = left;
			child_value = left_value;
		}

		if (child_value >= value) break;

		GenerationTile child_key = entropies->keys[child];
		entropies->keys[node] = child_key;
		entropies->tile_nodes[child_key] = node;
		entropies->values[node] = child_value;

		node = child;
	}

	entropies->keys[node] = key;
	entropies->tile_nodes[key] = node;
	entropies->values[node] = value;
}

void entropies_heap_push(Entropies* entropies, GenerationTile key, Entropy value) {
	entropies->heap_size++;

	entropies->keys[entropies->heap_size] = key;
	entropies->tile_nodes[key] = entropies->heap_size;
	entropies->values[entropies->heap_size] = value;

	entropies_heap_swim(entropies, entropies->heap_size);
}

void entropies_heap_set(Entropies* entropies, GenerationHeapNode node, Entropy value) {
	if (value > entropies->values[node]) {
		entropies->values[node] = value;
		entropies_heap_sink(entropies, node);
	} else {
		entropies->values[node] = value;
		entropies_heap_swim(entropies, node);
	}
}

void entropies_heapify(Entropies* entropies) {
	for (GenerationHeapNode i = entropies->heap_size >> 1; i >= 1; i--) {
		entropies_heap_sink(entropies, i);
	}
}

void entropies_update_entropy(Entropies* entropies, GenerationTile key, Entropy value) {
	int wasCollapsed = entropies->tiles[key] < 0;

	entropies->tiles[key] = value;
	if (wasCollapsed) {
		entropies_heap_push(entropies, key, value);
	} else {
		entropies_heap_set(entropies, entropies->tile_nodes[key], value);
	}
}

GenerationTile entropies_collapse_least(Entropies* entropies) {
	GenerationTile key = entropies->keys[1];

	entropies->tiles[key] = -1;

	GenerationTile last_key = entropies->keys[entropies->heap_size];
	entropies->keys[1] = last_key;
	entropies->tile_nodes[last_key] = 1;
	entropies->values[1] = entropies->values[entropies->heap_size];

	entropies->heap_size--;
	entropies_heap_sink(entropies, 1);

	return key;
}

void entropies_initalize_from_tiles(Entropies* entropies, int width, int height) {
	entropies->width = width;
	entropies->height = height;

	uint16_t size = width * height;
	uint16_t node = 1;
	for (uint16_t key = 0; key < size; key++) {
		int entropy = entropies->tiles[key];
		if (entropy < 0) continue;

		entropies->keys[node] = key;
		entropies->tile_nodes[key] = node;
		entropies->values[node] = entropy;
		node++;
	}

	entropies->heap_size = node - 1;
	entropies_heapify(entropies);
}

Entropies* entropies_create(int maxWidth, int maxHeight) {
	Entropies* entropies = malloc(sizeof(Entropies));

	if (entropies == NULL) {
		fprintf(stderr, "Failed to allocate memory: entropies_create()");
		exit(1);
	}

	// 2d array
	entropies->tiles = malloc(sizeof(int) * maxWidth * maxHeight);
	entropies->tile_nodes = malloc(sizeof(uint16_t) * maxWidth * maxHeight);

	// heap
	entropies->keys = malloc(sizeof(uint16_t) * maxWidth * maxHeight);
	entropies->values = malloc(sizeof(int) * maxWidth * maxHeight);

	if (entropies->tiles == NULL || entropies->tile_nodes == NULL || entropies->keys == NULL || entropies->values == NULL) {
		fprintf(stderr, "Failed to allocate memory: entropies_create()");
		exit(1);
	}

	return entropies;
}