#include "superposition.h"

typedef struct {
	uint16_t x;
	uint16_t y;
} GenerationTileCoords;

BitField temp_tile_field, temp_edge_field;

GenerationTileCoords generation_tile_to_coord(GenerationTile tile, int area_width) {
	GenerationTileCoords coords;

	coords.x = tile % area_width;
	coords.y = tile / area_width;

	return coords;
}

GenerationTile generation_coord_to_tile(GenerationTileCoords coords, int area_width) {
	return coords.x + coords.y * area_width;
}

// isCollapsed(world) {
// }

// // collapse tile with least entropy
// collapseLeast(world, fudgeChance = 0) {
// }

// // update entropy data for recetly changed tiles
// updateStaleEntropies(world) {
// 	for (const key of this.staleEntropyKeys.values()) {
// 		const x = key % this.width;
// 		const y = Math.trunc(key / this.width);

// 		if (world.getTile(x + this.x, y + this.y) != = 0) continue;

// 		const entropy = shannonEntropy(this.sampleSets[key].values());
// 		this.entropies.updateEntropy(key, entropy);
// 	}

// 	this.staleEntropyKeys = new Set();
// }

void superposition_collapse_at(Superposition* superposition, int u, int v) {
}

void superposition_constrain_field(Superposition* superposition, int u, int v, BitField edge_constraint, int direction) {
	BitField tile_field = field_index_array(superposition->fields, superposition->tileset->tile_field_size, u + v * superposition->width);

	int inital_pop = field_popcnt(tile_field, superposition->tileset->tile_field_size);
	tileset_constrain_tile(superposition->tileset, tile_field, edge_constraint, direction);
	int final_pop = field_popcnt(tile_field, superposition->tileset->tile_field_size);

	// propagate if there is a change
	if (inital_pop != final_pop)
		superposition_constrain_neighbours(superposition, u, v, direction);
}

void superposition_constrain_neighbours(Superposition* superposition, int u, int v, int skip_direction) {
	BitField tile_field = field_index_array(superposition->fields, superposition->tileset->tile_field_size, u + v * superposition->width);

	if (skip_direction != 0) {
		tileset_find_tile_edge(superposition->tileset, tile_field, temp_edge_field, 0);
		superposition_constrain_field(superposition, u + 1, v, temp_edge_field, 2);
	}

	if (skip_direction != 1) {
		tileset_find_tile_edge(superposition->tileset, tile_field, temp_edge_field, 1);
		superposition_constrain_field(superposition, u, v - 1, temp_edge_field, 3);
	}

	if (skip_direction != 2) {
		tileset_find_tile_edge(superposition->tileset, tile_field, temp_edge_field, 2);
		superposition_constrain_field(superposition, u - 1, v, temp_edge_field, 0);
	}

	if (skip_direction != 3) {
		tileset_find_tile_edge(superposition->tileset, tile_field, temp_edge_field, 3);
		superposition_constrain_field(superposition, u, v + 1, temp_edge_field, 1);
	}
}

void get_naive_tile_field(Superposition* superposition, int u, int v, BitField tile_field) {
	int tile_id = world_get(superposition->world, superposition->x + u, superposition->y + v);

	if (tile_id == -1) {
		distribution_area_select(superposition->area, u, v);
		distribution_area_get_all_tiles(tile_field, superposition->tileset->tile_field_size);
	} else {
		field_clear(tile_field, superposition->tileset->tile_field_size);
		field_set_bit(tile_field, tile_id);
	}
}

void superposition_set_area(Superposition* superposition, DistributionArea* area, Tileset* tileset, World* world, int x, int y, int width, int height) {
	superposition->x = x;
	superposition->y = y;
	superposition->width = width;
	superposition->height = height;
	superposition->area = area;
	superposition->tileset = tileset;
	superposition->world = world;

	superposition->fields = field_create_junk_array(width * height, tileset->tile_field_size);
	temp_edge_field = field_create(tileset->edge_field_size);
	temp_tile_field = field_create(tileset->tile_field_size);

	// get naive values for each tile feild
	for (int u = 0; u < width; u++) {
		for (int v = 0; v < height; v++) {
			BitField tile_field = field_index_array(superposition->fields, tileset->tile_field_size, u * v * width);
			get_naive_tile_field(superposition, u, v, tile_field);
		}
	}

	// contrain tiles baced off horizontal edges
	for (int u = 0; u < width; u++) {
		get_naive_tile_field(superposition, u, -1, temp_tile_field);
		superposition_constrain_field(superposition, u, 0, temp_tile_field, 1);

		get_naive_tile_field(superposition, u, height, temp_tile_field);
		superposition_constrain_field(superposition, u, height - 1, temp_tile_field, 3);
	}

	// contrain tiles baced off vertical edges
	for (int v = 0; v < height; v++) {
		get_naive_tile_field(superposition, -1, v, temp_tile_field);
		superposition_constrain_field(superposition, 0, v, temp_tile_field, 2);

		get_naive_tile_field(superposition, width, v, temp_tile_field);
		superposition_constrain_field(superposition, width - 1, v, temp_tile_field, 0);
	}

	// contrain tiles baced off eachother
	for (int u = 0; u < width; u++) {
		for (int v = 0; v < height; v++) {
			superposition_constrain_neighbours(superposition, u, v, -1);
		}
	}

	// calculate entropies for each tile
	for (int u = 0; u < width; u++) {
		for (int v = 0; v < height; v++) {
			BitField tile_field = field_index_array(superposition->fields, tileset->tile_field_size, u * v * width);

			distribution_area_select(area, u, v);
			superposition->entropies->tiles[u + v * width] = distribution_area_get_shannon_entropy(tile_field);
		}
	}

	entropies_initalize_from_tiles(superposition->entropies, width, height);
}

Superposition* superposition_create(int maxWidth, int maxHeight) {
	Superposition* superposition = malloc(sizeof(Superposition));

	if (superposition == NULL) {
		fprintf(stderr, "Failed to allocate memory: superposition_create()");
		exit(1);
	}

	superposition->entropies = entropies_create(maxWidth, maxHeight);
	superposition->fields = NULL;
	superposition->stale_tile_count = 0;
}