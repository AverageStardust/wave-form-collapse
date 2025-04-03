#ifndef TILESET_GUARD
#define TILESET_GUARD

#include <emscripten.h>
#include <stdint.h>
#include <stdio.h>

#include "bitfield.h"
#include "meminst.h"

typedef struct {
	int edge_field_size;
	int tile_field_size;
	int tile_table_direction_size;
	int edge_table_byte_size;
	BitField tile_table;
	BitField edge_table;
	uint32_t* render_data_table;
} Tileset;

extern EMSCRIPTEN_KEEPALIVE Tileset* tileset_create(int edge_field_size, int tile_field_size);
void tileset_constrain_tile(Tileset* tileset, BitField tile_field, BitField edge_field, int direction);
void tileset_find_tile_edge(Tileset* tileset, BitField tile_field, BitField edge_field, int direction);
extern EMSCRIPTEN_KEEPALIVE void tileset_add_tile(Tileset* tileset, int tile, uint32_t render_data, int right_edge, int top_edge, int left_edge, int bottom_edge);
extern EMSCRIPTEN_KEEPALIVE void tileset_free(Tileset* tileset);

#endif