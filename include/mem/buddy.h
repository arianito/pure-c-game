#pragma once

#include <stddef.h>

typedef struct __attribute__((aligned(32), packed)) {
    void *head;
    void *tail;
    unsigned int total;
    unsigned int usage;
    unsigned int padding;
} BuddyMemory;

BuddyMemory *make_buddy(unsigned int order);

BuddyMemory *buddy_create(void *m, unsigned int order);

void buddy_destroy(BuddyMemory **self);

unsigned int buddy_size(unsigned int order);

void *buddy_alloc(BuddyMemory *self, unsigned int size);

char buddy_free(BuddyMemory *self, void **ptr);