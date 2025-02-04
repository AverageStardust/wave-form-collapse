#include "bitfield.h"

#define FRAME_SIZE sizeof(BitFieldFrame)
#define divCeil(a, b) ((a + b - 1) / b)

BitField field_create(int size) {
	BitField field = calloc(1, divCeil(size, FRAME_SIZE));

	if (field == NULL) {
		fprintf(stderr, "Failed to allocate memory: field_create()");
		exit(1);
	}

	return field;
}

BitField field_create_array(int count, int size) {
	BitField fields = calloc(count, divCeil(size, FRAME_SIZE));

	if (fields == NULL) {
		fprintf(stderr, "Failed to allocate memory: field_create_array()");
		exit(1);
	}

	return fields;
}

void field_copy(BitField field_dest, BitField field_src, int size) {
	memcpy(field_dest, field_src, (size + FRAME_SIZE - 1) / FRAME_SIZE);
}

void field_clear(BitField field, int size) {
	memset(field, 0, (size + FRAME_SIZE - 1) / FRAME_SIZE);
}

void field_or(BitField field_dest, BitField field_src, int size) {
	v128_t a, b;

	for (int i = 0; i < divCeil(size, FRAME_SIZE); i++) {
		a = wasm_v128_load(field_dest + i);
		b = wasm_v128_load(field_src + i);
		a = wasm_v128_or(a, b);
		wasm_v128_store(field_dest + i, a);
	}
}

void field_and(BitField field_dest, BitField field_src, int size) {
	v128_t a, b;

	for (int i = 0; i < divCeil(size, FRAME_SIZE); i++) {
		a = wasm_v128_load(field_dest + i);
		b = wasm_v128_load(field_src + i);
		a = wasm_v128_and(a, b);
		wasm_v128_store(field_dest + i, a);
	}
}

int field_popcnt(BitField field, int size) {
	v128_t a, b;
	int sum = 0;

	for (int i = 0; i < divCeil(size, FRAME_SIZE); i++) {
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

uint8_t field_get_byte(BitField field, int byte) {
	uint8_t *bytes = (uint8_t *)field;
	return bytes[byte];
}

void field_set_bit(BitField field, int bit) {
	uint8_t *byte = (uint8_t *)field;
	byte += bit / 8;
	*byte |= 1 << (bit % 8);
}

int field_get_rightmost_bit(BitField field, int size, int starting_index) {
	uint32_t *words = (uint32_t *)field;
	uint32_t mask = 0xFFFFFFFF << starting_index;

	for (int i = starting_index / sizeof(uint32_t); i < divCeil(size, sizeof(uint32_t)); i++) {
		if (words[i] == 0) continue;
		int zeros = __builtin_ctz(words[i] & mask);

		if (zeros < sizeof(uint32_t))
			return i * sizeof(uint32_t) + zeros;

		mask = 0xFFFFFFFF;
	}

	return -1;
}

void field_print(BitField field, int size) {
	uint32_t *words = (uint32_t *)field;

	for (int i = 0; i < divCeil(size, sizeof(uint32_t)); i++) {
		printf("%.8x ", words[i]);
	}
	printf("\n");
}