#include "bitfield.h"

void field_clear(int size, v128_t field[size]) {
	uint64_t *words = (uint64_t *)field;

	for (int i = 0; i < size * sizeof(v128_t) / sizeof(uint64_t); i++) {
		words[i] = 0;
	}
}

void field_or(int size, v128_t field_a[size], v128_t field_b[size]) {
	v128_t a, b;

	for (int i = 0; i < size; i++) {
		a = wasm_v128_load(field_a + i);
		b = wasm_v128_load(field_b + i);
		a = wasm_v128_or(a, b);
		wasm_v128_store(field_a + i, a);
	}
}

void field_and(int size, v128_t field_a[size], v128_t field_b[size]) {
	v128_t a, b;

	for (int i = 0; i < size; i++) {
		a = wasm_v128_load(field_a + i);
		b = wasm_v128_load(field_b + i);
		a = wasm_v128_and(a, b);
		wasm_v128_store(field_a + i, a);
	}
}

int field_popcnt(int size, v128_t field[size]) {
	v128_t a, b;
	int sum = 0;

	for (int i = 0; i < size; i++) {
		a = wasm_v128_load(field + i);
		a = wasm_i8x16_popcnt(a);
		b = wasm_i8x16_shuffle(a, a, 1, 0, 3, 2, 5, 4, 7, 6, 9, 8, 11, 10, 13, 12, 15, 14);
		a = wasm_u8x16_add_sat(a, b);
		b = wasm_i8x16_shuffle(a, a, 2, 3, 0, 1, 6, 7, 4, 5, 10, 11, 8, 9, 14, 15, 12, 13);
		a = wasm_u8x16_add_sat(a, b);
		b = wasm_i8x16_shuffle(a, a, 4, 5, 6, 7, 0, 1, 2, 3, 12, 13, 14, 15, 8, 9, 10, 11);
		a = wasm_u8x16_add_sat(a, b);
		b = wasm_i8x16_shuffle(a, a, 8, 9, 10, 11, 12, 13, 14, 15, 0, 1, 2, 3, 4, 5, 6, 7);
		a = wasm_u8x16_add_sat(a, b);
		sum += wasm_u8x16_extract_lane(a, 0);
	}

	return sum;
}

uint8_t field_get_byte(v128_t *field, int byte) {
	uint8_t *bytes = (uint8_t *)field;
	return bytes[byte];
}

void field_set_bit(v128_t *field, int bit) {
	uint8_t *byte = (uint8_t *)field;
	byte += bit / 8;
	*byte |= 1 << (bit % 8);
}

int field_get_rightmost_bit(int size, v128_t field[size], int starting_index) {
	uint32_t *words = (uint32_t *)field;
	uint32_t mask = 0xFFFFFFFF << starting_index;

	for (int i = starting_index / 32; i < size * sizeof(v128_t) / sizeof(uint32_t); i++) {
		if (words[i] == 0) continue;
		int zeros = __builtin_ctz(words[i] & mask);

		if (zeros < 32)
			return i * 32 + zeros;

		mask = 0xFFFFFFFF;
	}

	return -1;
}

void field_print(int size, v128_t field[size]) {
	uint32_t *words = (uint32_t *)field;

	for (int i = 0; i < size * sizeof(v128_t) / sizeof(uint32_t); i++) {
		printf("%.8x ", words[i]);
	}
	printf("\n");
}