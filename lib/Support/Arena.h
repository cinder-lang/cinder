#ifndef CINDER_IMPL__SUPPORT__ARENA_H
#define CINDER_IMPL__SUPPORT__ARENA_H


#include <cinder/Support/Arena.h>


typedef struct CndArenaBlock {
    struct CndArenaBlock* next;
    void* memory;
    size_t allocated, capacity;
} CndArenaBlock;

struct CndArena {
    CndAllocatorCallbacks allocator;
    CndArenaBlock* blocks;
    size_t block_size;
};


#endif /* CINDER_IMPL__SUPPORT__ARENA_H */
