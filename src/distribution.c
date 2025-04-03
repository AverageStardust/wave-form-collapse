#include "distribution.h"

// set of selected distributions
struct {
	Distribution* distributions[4];
	int length;
} set;

void distribution_free(Distribution* distribution) {
	free_inst(distribution->weights);
	free_inst(distribution->weight_table);
	free_inst(distribution->weight_log_weight_table);
	free_inst(distribution);
}

void distribution_area_free(DistributionArea* area) {
	free_inst(area->distributions);
	free_inst(area);
}

int distribution_pick_random_unweighted(BitField field) {
	int tile_count = 0;

	for (int i = 0; i < set.length; i++) {
		Distribution* distribution = set.distributions[i];

		for (int j = 0;;) {
			int tile = field_get_rightmost_bit(field, distribution->tile_field_size, j);
			if (tile == NO_MORE_BITS) break;

			tile_count++;
			j = tile + 1;
		}
	}

	int roll = rand() % tile_count;
	tile_count = 0;

	for (int i = 0; i < set.length; i++) {
		Distribution* distribution = set.distributions[i];

		for (int j = 0;;) {
			int tile = field_get_rightmost_bit(field, distribution->tile_field_size, j);
			if (tile == NO_MORE_BITS) break;

			tile_count++;
			if (tile_count > roll) return tile;
			j = tile + 1;
		}
	}

	fprintf(stderr, "Failed to select tile in distribution_pick_random_unweighted()\n");
	exit(1);
}

int distribution_pick_random_from_weighted_byte(Distribution* distribution, BitField field, int byte) {
	Entropy byte_weight = distribution->weight_table[byte * 256 + field_get_byte(field, byte)];
	Entropy roll = rand() % byte_weight;
	Entropy weight_sum = 0;

	for (int i = byte * 8;;) {
		int tile = field_get_rightmost_bit(field, distribution->tile_field_size, i);
		if (tile == NO_MORE_BITS || tile >= byte * 8 + 8) break;
		i = tile + 1;

		weight_sum += distribution->weights[tile];
		if (weight_sum > roll) return tile;
	}

	exit(1);
}

void distribution_area_get_all_tiles(BitField field, int field_size) {
	field_clear(field, field_size);
	for (int i = 0; i < set.length; i++) {
		Distribution* distribution = set.distributions[i];
		if (field_size < distribution->tile_field_size) {
			field_or(field, distribution->all_tiles, field_size);
		} else {
			field_or(field, distribution->all_tiles, distribution->tile_field_size);
		}
	}
}

Entropy distribution_area_get_shannon_entropy(BitField field) {
	Entropy weight_sum = 0, weight_log_weight_sum = 0;

	for (int i = 0; i < set.length; i++) {
		Distribution* distribution = set.distributions[i];

		for (int j = 0; j < distribution->tile_field_size; j++) {
			int index = j * 256 + field_get_byte(field, j);
			weight_sum += distribution->weight_table[index];
			weight_log_weight_sum += distribution->weight_log_weight_table[index];
		}
	}

	if (weight_sum == 0) return 0;

	int log_weight_sum = (int)(logf(weight_sum) * ENTROPY_ONE_POINT);
	return log_weight_sum - (weight_log_weight_sum / weight_sum);
}

int distribution_area_pick_random(BitField field) {
	Entropy weight_sum = 0;

	for (int i = 0; i < set.length; i++) {
		Distribution* distribution = set.distributions[i];

		for (int j = 0; j < distribution->tile_field_size; j++) {
			weight_sum += distribution->weight_table[j * 256 + field_get_byte(field, j)];
		}
	}

	if (weight_sum == 0)
		return distribution_pick_random_unweighted(field);

	Entropy roll = rand() % weight_sum;
	weight_sum = 0;

	for (int i = 0; i < set.length; i++) {
		Distribution* distribution = set.distributions[i];

		for (int j = 0; j < distribution->tile_field_size; j++) {
			weight_sum += distribution->weight_table[j * 256 + field_get_byte(field, j)];

			if (weight_sum > roll)
				return distribution_pick_random_from_weighted_byte(distribution, field, j);
		}
	}

	fprintf(stderr, "Failed to select tile in distribution_pick_random()\n");
	exit(1);
}

void distribution_area_select(DistributionArea* area, int x, int y) {
	int start_u = (x * 4 / area->distribution_size + 1) / 4;
	int end_u = (x * 4 / area->distribution_size + 7) / 4;
	int start_v = (y * 4 / area->distribution_size + 1) / 4;
	int end_v = (y * 4 / area->distribution_size + 7) / 4;

	set.length = 0;

	for (int u = start_u; u < end_u; u++) {
		for (int v = start_v; v < end_v; v++) {
			Distribution* distribution = area->distributions[u + v * area->distributions_width];
			set.distributions[set.length] = distribution;
			set.length++;
		}
	}
}

void distribution_add_tile(Distribution* distribution, int tile, Entropy weight) {
	distribution->weights[tile] = weight;

	int tile_byte_index = tile / 8;
	int tile_bit_index = tile % 8;

	Entropy* weight_table = distribution->weight_table + tile_byte_index * 256;
	Entropy* weight_log_weight_table = distribution->weight_log_weight_table + tile_byte_index * 256;

	Entropy weight_log_weight = weight * (int)(logf(weight) * ENTROPY_ONE_POINT);

	// iterate though all byte values with tile_bit_index set
	for (int i = 0; i < 256; i += (2 << tile_bit_index)) {
		for (int j = 0; j < (1 << tile_bit_index); j++) {
			int byte = (1 << tile_bit_index) + i + j;
			weight_table[byte] += weight;
			weight_log_weight_table[byte] += weight_log_weight;
		}
	}

	field_set_bit(distribution->all_tiles, tile);
}

DistributionArea* distribution_area_create(Distribution** distributions, int distribution_size, int distributions_width) {
	DistributionArea* area = malloc_inst(sizeof(DistributionArea));
	if (area == NULL) {
		fprintf(stderr, "Failed to allocate memory: distribution_area_create()\n");
		exit(1);
	}

	area->distributions = distributions;
	area->distribution_size = distribution_size;
	area->distributions_width = distributions_width;

	return area;
}

Distribution* distribution_create(int tile_field_size) {
	Distribution* distribution = malloc_inst(sizeof(Distribution));

	if (distribution == NULL) {
		fprintf(stderr, "Failed to allocate memory: distribution_create()\n");
		exit(1);
	}

	distribution->weights = calloc_inst(tile_field_size * 256, sizeof(Entropy));
	distribution->weight_table = calloc_inst(tile_field_size * 256, sizeof(Entropy));
	distribution->weight_log_weight_table = calloc_inst(tile_field_size * 256, sizeof(Entropy));
	distribution->all_tiles = calloc_inst(1, tile_field_size);

	if (distribution->weights == NULL || distribution->weight_table == NULL || distribution->weight_log_weight_table == NULL || distribution->all_tiles == NULL) {
		fprintf(stderr, "Failed to allocate memory: distribution_create()\n");
		exit(1);
	}

	distribution->tile_field_size = tile_field_size;

	return distribution;
}