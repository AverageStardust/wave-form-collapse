#ifndef ENTROPIES_GUARD
#define ENTROPIES_GUARD

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "distribution.h"

typedef uint32_t GenerationHeapNode;
typedef uint32_t GenerationTile;

typedef struct {
	Entropy* tiles;
	GenerationHeapNode* tile_nodes;
	GenerationTile* keys;
	Entropy* values;
	int width;
	int height;
	GenerationHeapNode heap_size;
} Entropies;

// ported from JS
// holds entropy data for Superposition
// allow quick to:
//   - update entropy by coordinate with a 2d array
//   - to find minimum entropy with a heap
// these two data structures are updated in sync

Entropies* entropies_create(int maxWidth, int maxHeight);
void entropies_initalize_from_tiles(Entropies* entropies, int width, int height);
GenerationTile entropies_collapse_least(Entropies* entropies);
int entropies_is_collapsed(Entropies* entropies, GenerationTile key);
void entropies_update_entropy(Entropies* entropies, GenerationTile key, Entropy value);
void entropies_free(Entropies* entropies);

#endif