#ifndef DISTRIBUTION_GUARD
#define DISTRIBUTION_GUARD

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitfield.h"

// entropy is calculated with fixed point math, this is the integer value representing one
#define ENTROPY_ONE_POINT 1000

typedef struct {
	int* weights;
	int* weight_table;
	int* weight_log_weight_table;
	int tile_count;
	int tile_field_bytes;
	int tile_field_size;
} Distribution;

Distribution* distribution_create(int tile_field_size);
void distribution_add_tile(Distribution* distribution, int tile, int weight);
int distribution_pick_random(Distribution* distribution, BitField field);
int distributions_pick_random(int distribution_count, Distribution* distributions[distribution_count], BitField field);
int distribution_get_shannon_entropy(Distribution* distribution, BitField field);
int distributions_get_shannon_entropy(int distribution_count, Distribution* distributions[distribution_count], BitField field);
void distribution_free(Distribution* distribution);

#endif