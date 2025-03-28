#ifndef DISTRIBUTION_GUARD
#define DISTRIBUTION_GUARD

#include <emscripten.h>
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

extern EMSCRIPTEN_KEEPALIVE Distribution* distribution_create(int tile_field_size);
extern EMSCRIPTEN_KEEPALIVE void distribution_add_tile(Distribution* distribution, int tile, Entropy weight);
extern EMSCRIPTEN_KEEPALIVE void distribution_free(Distribution* distribution);

typedef struct {
	Distribution** distributions;
	int distribution_size;	  // size of an individual distribution in tiles
	int distributions_width;  // number of distributions wide
} DistributionArea;

extern EMSCRIPTEN_KEEPALIVE DistributionArea* distribution_area_create(Distribution** distributions, int distribution_size, int distributions_width);
void distribution_area_set_point(DistributionArea* area, Distribution* distribution, int x, int y);
void distribution_area_select(DistributionArea* area, int x, int y);
int distribution_area_pick_random(BitField field);
Entropy distribution_area_get_shannon_entropy(BitField field);
void distribution_area_get_all_tiles(BitField field, int field_size);
extern EMSCRIPTEN_KEEPALIVE void distribution_area_free(DistributionArea* area);

#endif