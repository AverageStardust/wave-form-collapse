#include "list.h"

void intlist_free(IntList* list) {
	free(list->elements);
	free(list);
}

void intlist_allocate_length(IntList* list, int min_length) {
	if (list->allocated_length >= min_length) return;

	list->elements = realloc(list->elements, list->allocated_length * 2 * sizeof(int));

	if (list == NULL) {
		fprintf(stderr, "Failed to allocate memory: intlist_allocate_length()");
		exit(1);
	}
}

void intlist_push(IntList* list, int value) {
	intlist_allocate_length(list, list->length + 1);

	list->elements[list->length] = value;
	list->length ++;
}

IntList* intlist_create(int inital_length) {
	IntList* list = malloc(sizeof(IntList));

	if (list == NULL) {
		fprintf(stderr, "Failed to allocate memory: intlist_create()");
		exit(1);
	}

	list->elements = calloc(inital_length, sizeof(int));

	if (list->elements == NULL) {
		fprintf(stderr, "Failed to allocate memory: intlist_create()");
		exit(1);
	}

	list->length = 0;
	list->allocated_length = inital_length;

	return list;
}