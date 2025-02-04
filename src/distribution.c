#include "distribution.h"

void distribution_free(Distribution* distribution) {
	free(distribution->weights);
	free(distribution->weight_table);
	free(distribution->weight_log_weight_table);
	free(distribution);
}

void distributions_set_all_tiles(int distribution_count, Distribution* distributions[distribution_count], BitField field) {
	for (int i = 0; i < distribution_count; i++) {
		Distribution* distribution = distributions[i];
		field_or(field, distribution->all_tiles, distribution->tile_field_size);
	}
}

Entropy distributions_get_shannon_entropy(int distribution_count, Distribution* distributions[distribution_count], BitField field) {
	Entropy weight_sum = 0, weight_log_weight = 0;

	for (int i = 0; i < distribution_count; i++) {
		Distribution* distribution = distributions[i];

		for (int j = 0; j < distribution->tile_field_size; j++) {
			int index = j * 256 + field_get_byte(field, j);
			weight_sum += distribution->weight_table[index];
			weight_log_weight += distribution->weight_log_weight_table[index];
		}
	}

	if (weight_sum == 0) return 0;

	return (int)(logf(weight_sum) * ENTROPY_ONE_POINT) - (weight_log_weight / weight_sum);
}

Entropy distribution_get_shannon_entropy(Distribution* distribution, BitField field) {
	return distributions_get_shannon_entropy(1, (Distribution*[]){distribution}, field);
}

int distribution_pick_random_unweighted(int distribution_count, Distribution* distributions[distribution_count], BitField field) {
	int tile_count = 0;

	for (int i = 0; i < distribution_count; i++) {
		Distribution* distribution = distributions[i];

		for (int j = 0;;) {
			int tile = field_get_rightmost_bit(field, distribution->tile_field_size, j);
			if (tile == -1) break;

			tile_count++;
			j = tile + 1;
		}
	}

	int roll = rand() % tile_count;
	tile_count = 0;

	for (int i = 0; i < distribution_count; i++) {
		Distribution* distribution = distributions[i];

		for (int j = 0;;) {
			int tile = field_get_rightmost_bit(field, distribution->tile_field_size, j);
			if (tile == -1) break;

			tile_count++;
			if (tile_count > roll) return tile;
			j = tile + 1;
		}
	}

	fprintf(stderr, "Failed to select tile in distribution_pick_random_unweighted()");
	exit(1);
}

int distribution_pick_random_from_weighted_byte(Distribution* distribution, BitField field, int byte) {
	Entropy weight_sum = distribution->weight_table[byte * 256 + field_get_byte(field, byte)];

	Entropy roll = rand() % weight_sum;
	weight_sum = 0;

	for (int i = byte * 8;;) {
		int tile = field_get_rightmost_bit(field, distribution->tile_field_size, i);
		if (tile == -1 || tile >= byte * 8 + 8) break;

		weight_sum += distribution->weights[tile];
		if (weight_sum > roll) return tile;
		i = tile + 1;
	}

	fprintf(stderr, "Failed to select tile in distribution_pick_random_from_weighted_byte()");
	exit(1);
}

int distributions_pick_random(int distribution_count, Distribution* distributions[distribution_count], BitField field) {
	Entropy weight_sum = 0;

	for (int i = 0; i < distribution_count; i++) {
		for (int j = 0; j < distributions[i]->tile_field_size; j++) {
			weight_sum += distributions[i]->weight_table[j * 256 + field_get_byte(field, j)];
		}
	}

	if (weight_sum == 0)
		return distribution_pick_random_unweighted(distribution_count, distributions, field);

	Entropy roll = rand() % weight_sum;
	weight_sum = 0;

	for (int i = 0; i < distribution_count; i++) {
		for (int j = 0; j < distributions[i]->tile_field_size; j++) {
			weight_sum += distributions[i]->weight_table[j * 256 + field_get_byte(field, j)];

			if (weight_sum > roll)
				return distribution_pick_random_from_weighted_byte(distributions[i], field, j);
		}
	}

	fprintf(stderr, "Failed to select tile in distribution_pick_random()");
	exit(1);
}

int distribution_pick_random(Distribution* distribution, BitField field) {
	return distributions_pick_random(1, (Distribution*[]){distribution}, field);
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

Distribution* distribution_create(int tile_field_size) {
	Distribution* distribution = malloc(sizeof(Distribution));

	if (distribution == NULL) {
		fprintf(stderr, "Failed to allocate memory: distribution_create()");
		exit(1);
	}

	distribution->weights = calloc(tile_field_size * 256, sizeof(Entropy));
	distribution->weight_table = calloc(tile_field_size * 256, sizeof(Entropy));
	distribution->weight_log_weight_table = calloc(tile_field_size * 256, sizeof(Entropy));
	distribution->all_tiles = calloc(1, tile_field_size);

	if (distribution->weights == NULL || distribution->weight_table == NULL || distribution->weight_log_weight_table == NULL || distribution->all_tiles == NULL) {
		fprintf(stderr, "Failed to allocate memory: distribution_create()");
		exit(1);
	}

	distribution->tile_field_size = tile_field_size;

	return distribution;
}