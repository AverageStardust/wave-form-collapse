#include "list.h"

void list_free(ListAbstract* list) {
	free_inst(list->elements);
	free_inst(list);
}

void list_allocate_length(ListAbstract* list, int min_length, int element_size) {
	if (list->allocated_length >= min_length) return;

	list->allocated_length *= 2;
	list->elements = realloc_inst(list->elements, list->allocated_length * element_size);

	if (list == NULL) {
		fprintf(stderr, "Failed to allocate memory: list_allocate_length()\n");
		exit(1);
	}
}

void list32_push(List32* list, uint32_t value) {
	list_allocate_length(list, list->length + 1, 4);

	*(uint32_t*)(list->elements + list->length * 4) = value;
	list->length++;
}

void list64_push(List64* list, uint64_t value) {
	list_allocate_length(list, list->length + 1, 8);

	*(uint64_t*)(list->elements + list->length * 8) = value;
	list->length++;
}

uint32_t list32_at(List32* list, int index) {
	return *(uint32_t*)(list->elements + index * 4);
}

uint64_t list64_at(List64* list, int index) {
	return *(uint64_t*)(list->elements + index * 8);
}

ListAbstract* list_create(int element_size, int inital_allocated_length) {
	ListAbstract* list = malloc_inst(sizeof(ListAbstract));

	if (list == NULL) {
		fprintf(stderr, "Failed to allocate memory: list_create()\n");
		exit(1);
	}

	list->elements = malloc_inst(inital_allocated_length * element_size);

	if (list->elements == NULL) {
		fprintf(stderr, "Failed to allocate memory: list_create()\n");
		exit(1);
	}

	list->length = 0;
	list->allocated_length = inital_allocated_length;

	return list;
}

List32* list32_create(int inital_allocated_length) {
	return (List32*)list_create(4, inital_allocated_length);
}

List64* list64_create(int inital_allocated_length) {
	return (List64*)list_create(4, inital_allocated_length);
}