#include "cache.h"
#include <stdlib.h>
#include <string.h>



CacheBlock *cache_head = NULL;
CacheBlock *cache_tail = NULL;

void cache_init() {
    cache_head = cache_tail = NULL;
}

CacheBlock* cache_find(off_t offset) {
    CacheBlock *current = cache_head;
    while (current) {
        if (current->offset == offset) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void cache_move_to_front(CacheBlock *block) {
    if (cache_head == block) return;

    if (block->prev) block->prev->next = block->next;
    if (block->next) block->next->prev = block->prev;

    if (cache_tail == block) {
        cache_tail = block->prev;
    }

    block->next = cache_head;
    block->prev = NULL;
    if (cache_head) cache_head->prev = block;
    cache_head = block;

    if (!cache_tail) {
        cache_tail = block;
    }
}

void cache_add(off_t offset, const char *data) {
    CacheBlock *new_block = malloc(sizeof(CacheBlock));
    new_block->offset = offset;
    memcpy(new_block->data, data, BLOCK_SIZE);

    if (cache_head == NULL) {
        cache_head = cache_tail = new_block;
        new_block->prev = new_block->next = NULL;
    } else {
        new_block->next = cache_head;
        new_block->prev = NULL;
        cache_head->prev = new_block;
        cache_head = new_block;
    }

    if (cache_size() > CACHE_SIZE) {
        CacheBlock *old_tail = cache_tail;
        cache_tail = old_tail->prev;
        cache_tail->next = NULL;
        free(old_tail);
    }
}

int cache_size() {
    int size = 0;
    CacheBlock *current = cache_head;
    while (current) {
        size++;
        current = current->next;
    }
    return size;
}

void cache_free() {
    CacheBlock *current = cache_head;
    while (current) {
        CacheBlock *next = current->next;
        free(current);
        current = next;
    }
}
