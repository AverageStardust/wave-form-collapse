#include <emscripten.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	int length;
	int allocated_length;
	void* elements;
} ListAbstract;

typedef ListAbstract List32;
typedef ListAbstract List64;

extern EMSCRIPTEN_KEEPALIVE List32* list32_create(int inital_allocated_length);
extern EMSCRIPTEN_KEEPALIVE List64* list64_create(int inital_allocated_length);
uint32_t list_at32(List32* list, int index);
uint64_t list_at64(List64* list, int index);
extern EMSCRIPTEN_KEEPALIVE void list32_push(List32* list, uint32_t value);
extern EMSCRIPTEN_KEEPALIVE void list64_push(List64* list, uint64_t value);
extern EMSCRIPTEN_KEEPALIVE void list_free(ListAbstract* list);