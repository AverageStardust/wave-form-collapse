#ifndef TILESET_GUARD
#define TILESET_GUARD

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitfield.h"

typedef struct {
	BitField tile_table;
	BitField edge_table;
	int edge_field_size;
	int tile_field_size;
	int tile_table_direction_size;
	int edge_table_byte_size;
} Tileset;

Tileset* tileset_create(int edge_field_size, int tile_field_size);
void tileset_constrain_tile(Tileset* tileset, BitField tile_field, BitField edge_field, int direction);
void tileset_find_tile_edge(Tileset* tileset, BitField tile_field, BitField edge_field, int direction);
void tileset_add_tile(Tileset* tileset, int tile, int right_edge, int top_edge, int left_edge, int bottom_edge);
void tileset_free(Tileset* tileset);

#endif