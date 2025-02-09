#ifndef DISTRIBUTION_GUARD
#define DISTRIBUTION_GUARD

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitfield.h"

// entropy is calculated with fixed point math, this is the integer value representing one
#define ENTROPY_ONE_POINT 1000

typedef int Entropy;
typedef struct {
	Entropy* weights;
	Entropy* weight_table;
	Entropy* weight_log_weight_table;
	BitField all_tiles;
	int tile_field_size;
} Distribution;

typedef struct {
	Distribution** distributions;
	int length;
	int max_tile_field_size;
} DistributionSet;

Distribution* distribution_create(int tile_field_size);
void distribution_add_tile(Distribution* distribution, int tile, Entropy weight);
int distributions_pick_random(DistributionSet distSet, BitField field);
Entropy distributions_get_shannon_entropy(DistributionSet distSet, BitField field);
void distributions_set_all_tiles(DistributionSet distSet, BitField field);
void distribution_free(Distribution* distribution);

#endif