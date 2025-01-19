#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int* elements;
	int length;
	int allocated_length;
} IntList;

void intlist_free(IntList* list);
void intlist_push(IntList* list, int value);
IntList* intlist_create(int inital_length);