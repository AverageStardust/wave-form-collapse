#ifndef SUPERPOSITION_GUARD
#define SUPERPOSITION_GUARD

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitfield.h"
#include "distribution.h"
#include "entropies.h"
#include "hashmap.h"
#include "tileset.h"
#include "world.h"

#define STALE_TILE_LIMIT 256

typedef struct {
	DistributionArea* area;
	Tileset* tileset;
	World* world;

	BitField temp_tile_field;
	BitField temp_edge_field;
	BitField fields;
	Entropies* entropies;
	Hashmap* stale_entropy_tiles;
	int record_entropy_changes;

	int x;
	int y;
	int width;
	int height;

} Superposition;

int superposition_is_fully_collapsed(Superposition* superposition);
void superposition_collapse_least(Superposition* superposition);
void superposition_set_area(Superposition* superposition, DistributionArea* area, Tileset* tileset, World* world, int x, int y, int width, int height);
Superposition* superposition_create(int maxWidth, int maxHeight);

#endif