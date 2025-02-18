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
	int distribution_size;
	int distributions_width;
} DistributionArea;

Distribution* distribution_create(int tile_field_size);
DistributionArea* distribution_area_create_single(Distribution* distribution);
DistributionArea* distribution_area_create(int distribution_size, int distributions_width);
void distribution_add_tile(Distribution* distribution, int tile, Entropy weight);
void distribution_select_area(DistributionArea* area, int x, int y);
int distribution_pick_random(BitField field);
Entropy distribution_get_shannon_entropy(BitField field);
void distribution_set_all_tiles(BitField field);
void distribution_free(Distribution* distribution);

#endif