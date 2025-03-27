#include "superposition.h"

int superposition_is_fully_collapsed(Superposition* superposition) {
	return superposition->entropies->heap_size == 0;
}

// update the entory for one tile, the value of this node in the hashmap is the superposition
Superposition* update_stale_entropies_map_func(uint64_t key, Superposition* superposition) {
	int u = x_from_hashkey(key), v = y_from_hashkey(key);
	int tile_field_size = superposition->world->tileset->tile_field_size;
	BitField tile_field = field_index_array(superposition->fields, tile_field_size, u + v * superposition->width);

	// find entropy of tile giving distribution
	distribution_area_select(superposition->area, u, v);
	Entropy new_entropy = distribution_area_get_shannon_entropy(tile_field);

	entropies_update_entropy(superposition->entropies, u * v * superposition->width, new_entropy);

	// since this is a map function we should return the value of this node
	return superposition;
}

// update entropy data for recetly changed tiles
void update_stale_entropies(Superposition* superposition) {
	hashmap_map(superposition->stale_entropy_tiles, update_stale_entropies_map_func);

	// clear hashmap
	hashmap_clear(superposition->stale_entropy_tiles, 32);
}

// collapse tile with least entropy
void superposition_collapse_least(Superposition* superposition) {
	Tileset* tileset = superposition->world->tileset;

	// pick tile with least entropy
	GenerationTile least_tile = entropies_collapse_least(superposition->entropies);
	int u = least_tile % superposition->width, v = least_tile / superposition->width;

	// get field for tile
	BitField tile_field = field_index_array(superposition->fields, tileset->tile_field_size, u + v * superposition->width);

	// collapse to tile using weighted random
	distribution_area_select(superposition->area, u, v);
	int tile_id = distribution_area_pick_random(tile_field);

	// update world
	world_set(superposition->world, superposition->x + u, superposition->y + v, tile_id);

	// update field
	field_clear(tile_field, tileset->tile_field_size);
	field_set_bit(tile_field, tile_id);

	// propogate change to neighbours
	constrain_neighbours(superposition, u, v, -1);

	// clean up entropies for next pick
	update_stale_entropies(superposition);
}

void constrain_field(Superposition* superposition, int u, int v, BitField edge_constraint, int direction) {
	Tileset* tileset = superposition->world->tileset;
	BitField tile_field = field_index_array(superposition->fields, tileset->tile_field_size, u + v * superposition->width);

	int inital_pop = field_popcnt(tile_field, tileset->tile_field_size);
	tileset_constrain_tile(tileset, tile_field, edge_constraint, direction);
	int final_pop = field_popcnt(tile_field, tileset->tile_field_size);

	// check if there was a change
	if (inital_pop != final_pop) {
		// record that entorpy is stale, the update is delayed incase it is done repeatedly in a short time
		// it's convinent to give a pointer to superposition for later, see update_stale_entropies
		if (superposition->record_entropy_changes)
			hashmap_set(superposition->stale_entropy_tiles, hashkey_from_pair(u, v), superposition);

		// propogate change to neighbours
		constrain_neighbours(superposition, u, v, direction);
	}
}

void constrain_neighbours(Superposition* superposition, int u, int v, int skip_direction) {
	Tileset* tileset = superposition->world->tileset;
	BitField tile_field = field_index_array(superposition->fields, tileset->tile_field_size, u + v * superposition->width);

	if (skip_direction != 0) {
		tileset_find_tile_edge(tileset, tile_field, superposition->temp_edge_field, 0);
		constrain_field(superposition, u + 1, v, superposition->temp_edge_field, 2);
	}

	if (skip_direction != 1) {
		tileset_find_tile_edge(tileset, tile_field, superposition->temp_edge_field, 1);
		constrain_field(superposition, u, v - 1, superposition->temp_edge_field, 3);
	}

	if (skip_direction != 2) {
		tileset_find_tile_edge(tileset, tile_field, superposition->temp_edge_field, 2);
		constrain_field(superposition, u - 1, v, superposition->temp_edge_field, 0);
	}

	if (skip_direction != 3) {
		tileset_find_tile_edge(tileset, tile_field, superposition->temp_edge_field, 3);
		constrain_field(superposition, u, v + 1, superposition->temp_edge_field, 1);
	}
}

void get_naive_tile_field(Superposition* superposition, int u, int v, BitField tile_field) {
	int tile_id = world_get(superposition->world, superposition->x + u, superposition->y + v);
	Tileset* tileset = superposition->world->tileset;

	if (tile_id == -1) {
		distribution_area_select(superposition->area, u, v);
		distribution_area_get_all_tiles(tile_field, tileset->tile_field_size);
	} else {
		field_clear(tile_field, tileset->tile_field_size);
		field_set_bit(tile_field, tile_id);
	}
}

void superposition_set_area(Superposition* superposition, DistributionArea* area, int x, int y, int width, int height) {
	superposition->x = x;
	superposition->y = y;
	superposition->width = width;
	superposition->height = height;
	superposition->area = area;

	Tileset* tileset = superposition->world->tileset;
	superposition->temp_edge_field = field_create(tileset->edge_field_size);
	superposition->temp_tile_field = field_create(tileset->tile_field_size);
	superposition->fields = field_create_junk_array(width * height, tileset->tile_field_size);

	superposition->record_entropy_changes = 0;

	// get naive values for each tile feild
	for (int u = 0; u < width; u++) {
		for (int v = 0; v < height; v++) {
			BitField tile_field = field_index_array(superposition->fields, tileset->tile_field_size, u * v * width);
			get_naive_tile_field(superposition, u, v, tile_field);
		}
	}

	// contrain tiles baced off horizontal edges
	for (int u = 0; u < width; u++) {
		get_naive_tile_field(superposition, u, -1, superposition->temp_tile_field);
		constrain_field(superposition, u, 0, superposition->temp_tile_field, 1);

		get_naive_tile_field(superposition, u, height, superposition->temp_tile_field);
		constrain_field(superposition, u, height - 1, superposition->temp_tile_field, 3);
	}

	// contrain tiles baced off vertical edges
	for (int v = 0; v < height; v++) {
		get_naive_tile_field(superposition, -1, v, superposition->temp_tile_field);
		constrain_field(superposition, 0, v, superposition->temp_tile_field, 2);

		get_naive_tile_field(superposition, width, v, superposition->temp_tile_field);
		constrain_field(superposition, width - 1, v, superposition->temp_tile_field, 0);
	}

	// contrain tiles baced off eachother
	for (int u = 0; u < width; u++) {
		for (int v = 0; v < height; v++) {
			constrain_neighbours(superposition, u, v, -1);
		}
	}

	// calculate entropies for each tile
	for (int u = 0; u < width; u++) {
		for (int v = 0; v < height; v++) {
			int tile_id = world_get(superposition->world, superposition->x + u, superposition->y + v);

			if (tile_id != -1) {
				// already collapsed, skip entropy calculation
				superposition->entropies->tiles[u + v * width] = -1;
				continue;
			}

			BitField tile_field = field_index_array(superposition->fields, tileset->tile_field_size, u * v * width);

			distribution_area_select(area, u, v);
			Entropy entropy = distribution_area_get_shannon_entropy(tile_field);

			superposition->entropies->tiles[u + v * width] = entropy;
		}
	}

	entropies_initalize_from_tiles(superposition->entropies, width, height);

	hashmap_clear(superposition->stale_entropy_tiles, 32);
	superposition->record_entropy_changes = 1;
}

Superposition* superposition_create(World* world) {
	Superposition* superposition = malloc(sizeof(Superposition));

	if (superposition == NULL) {
		fprintf(stderr, "Failed to allocate memory: superposition_create()\n");
		exit(1);
	}

	superposition->world = world;
	superposition->entropies = entropies_create(world->chunk_size, world->chunk_size);
	superposition->fields = NULL;
	superposition->stale_entropy_tiles = hashmap_create(32);
}