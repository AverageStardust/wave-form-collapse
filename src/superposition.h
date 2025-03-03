#ifndef SUPERPOSITION_GUARD
#define SUPERPOSITION_GUARD

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitfield.h"
#include "distribution.h"
#include "entropies.h"
<<<<<<< HEAD
#include "generator.h"
=======
>>>>>>> 6340f91 (checkpoint)
#include "hashmap.h"
#include "tileset.h"
#include "world.h"

#define STALE_TILE_LIMIT 256

typedef struct {
<<<<<<< HEAD
	Entropies* entropies;

	BitField* tiles;

=======
	DistributionArea* area;
	BitField tiles;
	Entropies* entropies;
>>>>>>> 6340f91 (checkpoint)
	GenerationTile stale_entropy_tiles[STALE_TILE_LIMIT];
	int stale_tile_count;

	int x;
	int y;
	int width;
	int height;

} Superposition;

<<<<<<< HEAD
Superposition* superposition_create(Generator* generator, int maxWidth, int maxHeight);
=======
Superposition* superposition_create(int maxWidth, int maxHeight);
>>>>>>> 6340f91 (checkpoint)

#endif