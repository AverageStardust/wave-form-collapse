#ifndef BITFIELD_GUARD
#define BITFIELD_GUARD

#include <stdint.h>
#include <stdio.h>
#include <wasm_simd128.h>

#define BITFIELD_FRAME_SIZE sizeof(v128_t)

typedef v128_t *BitField;

void field_clear(int size, v128_t field[size]);
void field_or(int size, v128_t field_a[size], v128_t field_b[size]);
void field_and(int size, v128_t field_a[size], v128_t field_b[size]);
int field_popcnt(int size, v128_t field[size]);
uint8_t field_get_byte(v128_t *field, int byte);
void field_set_bit(v128_t *field, int bit);
int field_get_rightmost_bit(int size, v128_t field[size], int starting_index);
void field_print(int size, v128_t field[size]);

#endif