#include "Align.h"
#include "Alloc.h"
#include "Arena.h"

#include <assert.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>


static CndArenaBlock*
_cndArenaGetFirstSuitibleBlock(CndArenaRef arena, size_t size, size_t alignment) {
    assert(arena != nullptr);
    assert(size > 0);
    assert(alignment > 0);

    for (
        CndArenaBlock* block = arena->blocks;
        block != nullptr;
        block = block->next
    ) {
        size_t aligned_allocated = cndAlignTo(block->allocated, alignment);
        if (size <= block->capacity - aligned_allocated) {
            return block;
        }
    }

    size_t block_capacity = arena->block_size;
    assert(block_capacity != 0);

    if (block_capacity < size) {
        block_capacity = size;
    }

    void* block_memory = cndAllocate(arena->allocator, block_capacity);
    assert(block_memory != nullptr);

    CndArenaBlock* new_block = cndAllocate(arena->allocator, sizeof(*new_block));
    assert(new_block != nullptr);

    *new_block = (CndArenaBlock) {
        .next = arena->blocks,
        .memory = block_memory,
        .capacity = block_capacity,
    };

    arena->blocks = new_block;
    return new_block;
}


CNDAPI_ATTR CndArenaRef CNDAPI_CALL
cndArenaCreate(size_t block_size, CndAllocatorCallbacks* allocator) {
    if (block_size == 0) {
        block_size = 4 * 1024;
    }

    CndAllocatorCallbacks _allocator = {};
    if (allocator != nullptr) {
        _allocator = *allocator;
    } else {
        cndAllocatorCallbacksPopulateDefault(&_allocator);
    }

    assert(_allocator.allocate != nullptr);
    assert(_allocator.reallocate != nullptr);
    assert(_allocator.deallocate != nullptr);

    CndArenaRef arena = cndAllocate(_allocator, sizeof(*arena));
    assert(arena != nullptr);

    *arena = (struct CndArena) {
        .allocator = _allocator,
        .block_size = block_size,
    };

    return arena;
}


CNDAPI_ATTR void CNDAPI_CALL
cndArenaDestroy(CndArenaRef arena) {
    if (arena == nullptr) return;

    CndAllocatorCallbacks allocator = arena->allocator;
    for (
        CndArenaBlock* block = arena->blocks;
        block != nullptr;
        /* this increment intentionally left blank */
    ) {
        CndArenaBlock* next_block = block->next;
        cndDeallocate(allocator, block->memory, block->capacity);
        cndDeallocate(allocator, block, sizeof(*block));
        block = next_block;
    }

    cndDeallocate(allocator, arena, sizeof(*arena));
}


CNDAPI_ATTR void* CNDAPI_CALL
cndArenaAlloc(CndArenaRef arena, size_t size) {
    return cndArenaAllocAligned(arena, size, alignof(max_align_t));
}


CNDAPI_ATTR void* CNDAPI_CALL
cndArenaAllocAligned(CndArenaRef arena, size_t size, size_t alignment) {
    if (arena == nullptr || size == 0) return nullptr;

    alignment = cndAlignNormalize(alignment);
    assert(alignment > 0);
    size = cndAlignTo(size, alignment);

    CndArenaBlock* block = _cndArenaGetFirstSuitibleBlock(arena, size, alignment);
    assert(block != nullptr);

    size_t aligned_allocated = cndAlignTo(block->allocated, alignment);
    block->allocated = aligned_allocated + size;

    void* memory = ((char*) block->memory) + aligned_allocated;
    memset(memory, 0, size);

    return memory;
}


CNDAPI_ATTR void CNDAPI_CALL
cndArenaClear(CndArenaRef arena) {
    if (arena == nullptr) return;
    for (
        CndArenaBlock* block = arena->blocks;
        block != nullptr;
        block = block->next
    ) {
        block->allocated = 0;
    }
}
