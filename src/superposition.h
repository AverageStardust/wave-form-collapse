#ifndef SUPERPOSITION_GUARD
#define SUPERPOSITION_GUARD

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitfield.h"
#include "distribution.h"
#include "entropies.h"
#include "generator.h"
#include "hashmap.h"
#include "tileset.h"
#include "world.h"

#define STALE_TILE_LIMIT 256

typedef struct {
	Entropies* entropies;

	BitField* tiles;

	GenerationTile stale_entropy_tiles[STALE_TILE_LIMIT];
	int stale_tile_count;

	int x;
	int y;
	int width;
	int height;

} Superposition;

Superposition* superposition_create(Generator* generator, int maxWidth, int maxHeight);

#endif