#include "meminst.h"

size_t memory_usage = 0;

int get_memory_usage() {
#ifndef DO_MEMORY_INSTRUMENTATION
	fprintf(stderr, "Memory usage isn't being recorded, try -DDO_MEMORY_INSTRUMENTATION\n");
	exit(1);
#endif
	return memory_usage;
}

void add_memory(void* ptr) {
#ifdef DO_MEMORY_INSTRUMENTATION
	memory_usage += malloc_usable_size(ptr);
#endif
}

void remove_memory(void* ptr) {
#ifdef DO_MEMORY_INSTRUMENTATION
	memory_usage -= malloc_usable_size(ptr);
#endif
}

void* malloc_inst(size_t size) {
	void* ptr = malloc(size);
	add_memory(ptr);
	return ptr;
}

void* calloc_inst(size_t nmemb, size_t size) {
	void* ptr = calloc(nmemb, size);
	add_memory(ptr);
	return ptr;
}

void* realloc_inst(void* ptr, size_t size) {
	remove_memory(ptr);
	ptr = realloc(ptr, size);
	add_memory(ptr);
	return ptr;
}

void free_inst(void* ptr) {
	remove_memory(ptr);
	free(ptr);
}