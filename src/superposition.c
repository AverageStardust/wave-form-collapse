#include "superposition.h"

typedef struct {
	uint16_t x;
	uint16_t y;
} GenerationTileCoords;

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

// collapseAt(world, x, y) {
// }

// constrainNeighbours(x, y) {
// }

BitField superposition_get_naive_tile_superposition(Superposition* superposition, BitField tile, int u, int v) {
	World* world = superposition->generator->world;
	DistributionSet distSet = superposition->tile_distributions[u, v];

	int tile_id = world_get(world, superposition->x + u, superposition->y + v);

	if (tile_id == -1) {
		field_copy(tile, distSet.distributions[0]->all_tiles, distSet.distributions[0]->tile_field_size);
		for (int i = 1; i < distSet.length; i++) {
			field_or(tile, distSet.distributions[i]->all_tiles, distSet.distributions[i]->tile_field_size);
		}
	} else {
		field_set_bit(tile, tile_id);
	}

	return tile;
}

void superposition_set_area(Superposition* superposition, DistributionArea* area, int x, int y, int width, int height) {
	superposition->x = x;
	superposition->y = y;
	superposition->width = width;
	superposition->height = height;
	superposition->area = area;

	int tile_field_size = area->max_tile_field_size;

	// TODO: init tiles

	for (int u = 0; u < width; u++) {
		for (int v = 0; v < height; v++) {
			BitField tile = field_index_array(superposition->tiles, tile_field_size, u * v * width);
			superposition_get_naive_tile_superposition(superposition, tile, u, v);
		}
	}

	// TODO: constain neighbours

	for (int u = 0; u < width; u++) {
		for (int v = 0; v < height; v++) {
			BitField tile = field_index_array(superposition->tiles, tile_field_size, u * v * width);
			int population = field_popcnt(tile, tile_field_size);

			if (population == 1) {
				superposition->entropies->tiles[u + v * width] = -1;
			} else {
				DistributionSet distSet = superposition->tile_distributions[u + v * width];
				superposition->entropies->tiles[u + v * width] = distributions_get_shannon_entropy(distSet, tile);
			}
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
<<<<<<< HEAD
	superposition->generator = generator;
=======
>>>>>>> 6340f91 (checkpoint)
	superposition->tiles = NULL;
	superposition->stale_tile_count = 0;
}