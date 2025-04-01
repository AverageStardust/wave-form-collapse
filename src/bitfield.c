#include "bitfield.h"

#define bit_field_storage_byte_size(a) (bit_field_storage_frame_size(a) * BIT_FIELD_FRAME_SIZE)
#define bit_field_storage_type_size(a, b) (bit_field_storage_byte_size(a) / sizeof(b))

BitField field_create(int size) {
	BitField field = calloc(1, bit_field_storage_byte_size(size));

	if (field == NULL) {
		fprintf(stderr, "Failed to allocate memory: field_create()\n");
		exit(1);
	}

	return field;
}

BitField field_create_junk_array(int count, int elm_size) {
	BitField array = malloc(count * bit_field_storage_byte_size(elm_size));

	if (array == NULL) {
		fprintf(stderr, "Failed to allocate memory: field_create_array()\n");
		exit(1);
	}

	return array;
}

BitField field_create_empty_array(int count, int elm_size) {
	BitField array = calloc(count, bit_field_storage_byte_size(elm_size));

	if (array == NULL) {
		fprintf(stderr, "Failed to allocate memory: field_create_array()\n");
		exit(1);
	}

	return array;
}

BitField field_realloc_array(BitField array, int count, int elm_size) {
	array = realloc(array, count * bit_field_storage_byte_size(elm_size));

	if (array == NULL) {
		fprintf(stderr, "Failed to allocate memory: field_realloc_array()\n");
		exit(1);
	}

	return array;
}

BitField field_index_array(BitField array, int elm_size, int index) {
	return &array[bit_field_storage_frame_size(elm_size) * index];
}

void field_copy(BitField field_dest, BitField field_src, int size) {
	memcpy(field_dest, field_src, bit_field_storage_byte_size(size));
}

void field_clear(BitField field, int size) {
	memset(field, 0, bit_field_storage_byte_size(size));
}

void field_or(BitField field_dest, BitField field_src, int size) {
	v128_t a, b;

	for (int i = 0; i < bit_field_storage_frame_size(size); i++) {
		a = wasm_v128_load(field_dest + i);
		b = wasm_v128_load(field_src + i);
		a = wasm_v128_or(a, b);
		wasm_v128_store(field_dest + i, a);
	}
}

void field_and(BitField field_dest, BitField field_src, int size) {
	v128_t a, b;

	for (int i = 0; i < bit_field_storage_frame_size(size); i++) {
		a = wasm_v128_load(field_dest + i);
		b = wasm_v128_load(field_src + i);
		a = wasm_v128_and(a, b);
		wasm_v128_store(field_dest + i, a);
	}
}

int field_popcnt(BitField field, int size) {
	v128_t a, b;
	int sum = 0;

	for (int i = 0; i < bit_field_storage_frame_size(size); i++) {
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

	for (int i = starting_index / sizeof(uint32_t); i < bit_field_storage_type_size(size, uint32_t); i++) {
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

	for (int i = 0; i < bit_field_storage_type_size(size, uint32_t); i++) {
		printf("%.8x ", words[i]);
	}
	printf("\n");
}