#include "tileset.h"

void tileset_free(Tileset* tileset) {
	free(tileset->tile_table);
	free(tileset->edge_table);
	free(tileset);
}

void tileset_find_tile_edge(Tileset* tileset, BitField tile_field, BitField edge_field, int direction) {
	field_clear(edge_field, tileset->edge_field_size);

	// look up each byte in the tile_field, combine to find the edge_field for the tile
	for (int i = 0; i < tileset->tile_field_size * 16; i++) {
		const uint8_t byte = field_get_byte(tile_field, i);
		if (byte == 0) continue;  // no tiles set
		BitField edge_table_byte = tileset->edge_table + i * tileset->edge_table_byte_size;
		BitField byte_edge_field = edge_table_byte + ((byte * 4 + direction) * tileset->edge_field_size);
		field_or(edge_field, byte_edge_field, tileset->edge_field_size);
	}
}

void tileset_constrain_tile(Tileset* tileset, BitField tile_field, BitField edge_field, int direction) {
	BitField table = tileset->tile_table;
	table += tileset->tile_table_direction_size * direction;

	BitFieldFrame constraint[bit_field_storage_frame_size(tileset->tile_field_size)];

	// look up each byte in the edge_field, combine to find the constraint on tile_field
	for (int i = 0; i < tileset->edge_field_size * 16; i++) {
		const uint8_t byte = field_get_byte(edge_field, i);
		if (byte == 0) continue;  // no edges set
		BitField byte_tile_field = table + (i * 256 + byte) * tileset->tile_field_size;
		field_or(constraint, byte_tile_field, tileset->tile_field_size);
	}

	field_and(tile_field, constraint, tileset->tile_field_size);
}

void tileset_add_edge_table_entry(Tileset* tileset, int tile, int right_edge, int top_edge, int left_edge, int bottom_edge) {
	int tile_byte_index = tile / 8;
	int tile_bit_index = tile % 8;

	BitField table = tileset->edge_table;
	table += tileset->edge_table_byte_size * tile_byte_index;

	// iterate though all byte values with tile_bit_index set
	for (int i = 0; i < 256; i += (2 << tile_bit_index)) {
		for (int j = 0; j < (1 << tile_bit_index); j++) {
			int byte = (1 << tile_bit_index) + i + j;
			BitField byte_edge_table = table + byte * 4 * tileset->edge_field_size;
			field_set_bit(byte_edge_table + tileset->edge_field_size * 0, right_edge);
			field_set_bit(byte_edge_table + tileset->edge_field_size * 1, top_edge);
			field_set_bit(byte_edge_table + tileset->edge_field_size * 2, left_edge);
			field_set_bit(byte_edge_table + tileset->edge_field_size * 3, bottom_edge);
		}
	}
}

void tileset_add_tile_table_direction_entry(Tileset* tileset, int direction, int tile, int edge) {
	int edge_byte_index = edge / 8;
	int edge_bit_index = edge % 8;

	BitField table = tileset->tile_table;
	table += tileset->tile_table_direction_size * direction;
	table += edge_byte_index * 256 * tileset->tile_field_size;

	// iterate though all byte values with edge_bit_index set
	for (int i = 0; i < 256; i += (2 << edge_bit_index)) {
		for (int j = 0; j < (1 << edge_bit_index); j++) {
			int byte = (1 << edge_bit_index) + i + j;
			BitField tile_field = table + byte * tileset->tile_field_size;
			field_set_bit(tile_field, tile);
		}
	}
}

void tileset_add_tile_table_entry(Tileset* tileset, int tile, int right_edge, int top_edge, int left_edge, int bottom_edge) {
	tileset_add_tile_table_direction_entry(tileset, 0, tile, right_edge);
	tileset_add_tile_table_direction_entry(tileset, 1, tile, top_edge);
	tileset_add_tile_table_direction_entry(tileset, 2, tile, left_edge);
	tileset_add_tile_table_direction_entry(tileset, 3, tile, bottom_edge);
}

void tileset_add_tile(Tileset* tileset, int tile, int right_edge, int top_edge, int left_edge, int bottom_edge) {
	tileset_add_tile_table_entry(tileset, tile, right_edge, top_edge, left_edge, bottom_edge);
	tileset_add_edge_table_entry(tileset, tile, right_edge, top_edge, left_edge, bottom_edge);
}

Tileset* tileset_create(int edge_field_size, int tile_field_size) {
	int tile_table_direction_size = edge_field_size * 256 * tile_field_size;
	int edge_table_byte_size = 256 * 4 * edge_field_size;

	Tileset* tileset = malloc(sizeof(Tileset));

	if (tileset == NULL) {
		fprintf(stderr, "Failed to allocate memory: tileset_create()");
		exit(1);
	}

	tileset->tile_table = calloc(4 * tile_table_direction_size, sizeof(BitFieldFrame));
	tileset->edge_table = calloc(tile_field_size * edge_table_byte_size, sizeof(BitFieldFrame));

	if (tileset->tile_table == NULL || tileset->edge_table == NULL) {
		fprintf(stderr, "Failed to allocate memory: tileset_create()");
		exit(1);
	}

	tileset->edge_field_size = edge_field_size;
	tileset->tile_field_size = tile_field_size;
	tileset->tile_table_direction_size = tile_table_direction_size;
	tileset->edge_table_byte_size = edge_table_byte_size;

	return tileset;
}
