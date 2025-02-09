#ifndef BITFIELD_GUARD
#define BITFIELD_GUARD

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wasm_simd128.h>

typedef v128_t BitFieldFrame;
typedef BitFieldFrame* BitField;

BitField field_create(int size);
BitField field_create_array(int count, int elm_size);
BitField field_index_array(BitField array, int elm_size, int index);
void field_copy(BitField field_dest, BitField field_src, int size);
void field_clear(BitField field, int size);
void field_or(BitField field_dest, BitField field_src, int size);
void field_and(BitField field_dest, BitField field_src, int size);
int field_popcnt(BitField field, int size);
uint8_t field_get_byte(BitField field, int byte);
void field_set_bit(BitField field, int bit);
int field_get_rightmost_bit(BitField field, int size, int starting_index);
void field_print(BitField field, int size);

#endif