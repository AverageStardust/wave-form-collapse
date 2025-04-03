#ifndef MEMINST_GUARD
#define MEMINST_GUARD

#include <emscripten.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

EMSCRIPTEN_KEEPALIVE extern void* malloc_inst(size_t size);
EMSCRIPTEN_KEEPALIVE extern void* calloc_inst(size_t nmemb, size_t size);
EMSCRIPTEN_KEEPALIVE extern void* realloc_inst(void* ptr, size_t size);
EMSCRIPTEN_KEEPALIVE extern void free_inst(void* ptr);
EMSCRIPTEN_KEEPALIVE extern int get_memory_usage();

#endif