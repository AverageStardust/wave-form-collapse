#include "hashmap.h"

// one at a time
uint32_t jenkins_hash(int key_length, uint8_t* key) {
	uint32_t hash = 0xdeadbeef;	 // something to stop 0 hashing to 0

	for (int i = 0; i < key_length; i++) {
		hash += key[i];
		hash += hash << 10;
		hash ^= hash >> 6;
	}

	hash += hash << 3;
	hash ^= hash >> 11;
	hash += hash << 15;

	return hash;
}

HashmapNode* hashmap_create_node(uint64_t key, void* value) {
	HashmapNode* node = malloc(sizeof(HashmapNode));

	if (node == NULL) {
		fprintf(stderr, "Failed to allocate memory: hashmap_create_node()");
		exit(1);
	}

	node->next = NULL;
	node->value = value;
	node->key = key;

	return node;
}

HashmapNode* hashmap_remove_node(Hashmap* hashmap, uint64_t key, int index) {
	HashmapNode** node_link = &hashmap->nodes[index];
	HashmapNode* node = hashmap->nodes[index];
	int is_deleting_collision = 0;

	while (node != NULL) {
		if (node->key == key) {
			if (node->next) {
				*node_link = node->next;
			} else {
				*node_link = NULL;
			}

			if (is_deleting_collision) {
				hashmap->collisions--;
			}

			node->next = NULL;
			return node;
		} else {
			node_link = &node->next;
			node = node->next;
			is_deleting_collision = 1;
		}
	}

	return NULL;
}

void hashmap_add_node(Hashmap* hashmap, HashmapNode* node, int index) {
	if (hashmap->nodes[index] == NULL) {
		hashmap->nodes[index] = node;
	} else {
		node->next = hashmap->nodes[index];
		hashmap->nodes[index] = node;
		hashmap->collisions++;
	}
}

void hashmap_grow_bucket(Hashmap* hashmap, HashmapNode* node, int old_size, int new_size) {
	if (node == NULL) return;
	hashmap_grow_bucket(hashmap, node->next, old_size, new_size);

	uint32_t hash = jenkins_hash(sizeof(uint64_t), (uint8_t*)&node->key);
	if (hash % new_size == hash % old_size) return;

	hashmap_remove_node(hashmap, node->key, hash % old_size);
	hashmap_add_node(hashmap, node, hash % new_size);
}

void hashmap_grow(Hashmap* hashmap, int new_size) {
	hashmap->nodes = realloc(hashmap->nodes, new_size * sizeof(HashmapNode*));

	if (hashmap->nodes == NULL) {
		fprintf(stderr, "Failed to allocate memory: hashmap_grow()");
		exit(1);
	}

	for (int i = 0; i < hashmap->size; i++) {
		hashmap_grow_bucket(hashmap, hashmap->nodes[i], hashmap->size, new_size);
	}

	hashmap->size = new_size;
}

void hashmap_free(Hashmap* hashmap, void (*free_value)(void* value)) {
	for (int i = 0; i < hashmap->size; i++) {
		while (hashmap->nodes[i] != NULL) {
			void* value = hashmap_delete(hashmap, hashmap->nodes[i]->key);
			if (free_value == NULL) {
				free(value);
			} else {
				free_value(value);
			}
		}
	}

	free(hashmap->nodes);
	free(hashmap);
}

void hashmap_clear(Hashmap* hashmap, int new_size) {
	hashmap->size = new_size;
	hashmap->collisions = 0;

	free(hashmap->nodes);
	hashmap->nodes = calloc(new_size, sizeof(HashmapNode*));

	if (hashmap->nodes == NULL) {
		fprintf(stderr, "Failed to allocate memory: hashmap_clear()");
		exit(1);
	}
}

void hashmap_map(Hashmap* hashmap, void* (*map_func)(uint64_t key, void* value)) {
	for (int i = 0; i < hashmap->size; i++) {
		HashmapNode* node = hashmap->nodes[i];

		while (node != NULL) {
			node->value = map_func(node->key, node->value);
			node = node->next;
		}
	}
}

void* hashmap_delete(Hashmap* hashmap, uint64_t key) {
	uint32_t hash = jenkins_hash(sizeof(uint64_t), (uint8_t*)&key);
	HashmapNode* node = hashmap_remove_node(hashmap, key, hash % hashmap->size);
	void* value = node->value;

	free(node);

	return value;
}

void* hashmap_get(Hashmap* hashmap, uint64_t key) {
	uint32_t hash = jenkins_hash(sizeof(uint64_t), (uint8_t*)&key);
	int index = hash % hashmap->size;

	HashmapNode* node = hashmap->nodes[index];

	while (node != NULL) {
		if (node->key == key) {
			return node->value;
		} else {
			node = node->next;
		}
	}

	return NULL;
}

int hashmap_has(Hashmap* hashmap, uint64_t key) {
	uint32_t hash = jenkins_hash(sizeof(uint64_t), (uint8_t*)&key);
	int index = hash % hashmap->size;

	HashmapNode* node = hashmap->nodes[index];

	while (node != NULL) {
		if (node->key == key) {
			return 1;
		} else {
			node = node->next;
		}
	}

	return 0;
}

void* hashmap_set(Hashmap* hashmap, uint64_t key, void* value) {
	uint32_t hash = jenkins_hash(sizeof(uint64_t), (uint8_t*)&key);
	int index = hash % hashmap->size;

	if (hashmap->nodes[index] == NULL) {
		hashmap->nodes[index] = hashmap_create_node(key, value);
		return NULL;
	} else {
		HashmapNode* node = hashmap->nodes[index];

		while (node->key != key) {
			if (node->next == NULL) {
				node->next = hashmap_create_node(key, value);

				hashmap->collisions++;
				if (hashmap->collisions >= hashmap->size)
					hashmap_grow(hashmap, hashmap->size * 2);

				return NULL;
			}

			node = node->next;
		}

		void* old_value = node->value;
		node->value = value;
		return old_value;
	}
}

Hashmap* hashmap_create(int inital_size) {
	Hashmap* hashmap = malloc(sizeof(Hashmap));

	if (hashmap == NULL) {
		fprintf(stderr, "Failed to allocate memory: hashmap_create()");
		exit(1);
	}

	hashmap->size = inital_size;
	hashmap->collisions = 0;
	hashmap->nodes = calloc(inital_size, sizeof(HashmapNode*));

	if (hashmap->nodes == NULL) {
		fprintf(stderr, "Failed to allocate memory: hashmap_create()");
		exit(1);
	}

	return hashmap;
}