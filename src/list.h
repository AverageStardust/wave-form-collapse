#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int length;
	int allocated_length;
	int* elements;
} IntList;

void intlist_free(IntList* list);
void intlist_push(IntList* list, int value);
IntList* intlist_create(int inital_allocated_length);