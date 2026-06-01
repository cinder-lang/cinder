#include <assert.h>

#include "Alloc.h"

#if defined(_WIN32)
#  include <malloc.h>
#endif

#include <stdalign.h>
#include <stddef.h>
#include <stddefer.h>
#include <stdlib.h>
#include <string.h>


CNDAPI_ATTR CndAllocLayout CNDAPI_CALL
cndAllocLayout(size_t size, size_t alignment) {
    if (size == 0) {
        size = 1;
    }

    if (alignment == 0) {
        alignment = alignof(max_align_t);
    } else {
        alignment = cndAlignNormalize(alignment);
    }

    return (CndAllocLayout) {
        .size = cndAlignTo(size, alignment),
        .alignment = alignment,
    };
}


static void*
_cndDefaultAllocate(
    void* userdata,
    CndAllocLayout layout
) {
    assert(layout.size > 0);
    assert(layout.alignment > 0);
    assert(layout.size % layout.alignment == 0);

#if defined(_WIN32)
    return _aligned_malloc(layout.size, layout.alignment);
#else
    return aligned_alloc(layout.alignment, layout.size);
#endif
}


static void*
_cndDefaultReallocate(
    void* userdata,
    void* memory,
    CndAllocLayout previous_layout,
    CndAllocLayout new_layout
) {
    assert(memory != nullptr);

    assert(previous_layout.size > 0);
    assert(previous_layout.alignment > 0);
    assert(previous_layout.size % previous_layout.alignment == 0);

    assert(new_layout.size > 0);
    assert(new_layout.alignment > 0);
    assert(new_layout.size % new_layout.alignment == 0);

    assert(previous_layout.alignment <= new_layout.alignment);

    if (
        previous_layout.size <= new_layout.size &&
        previous_layout.alignment == new_layout.alignment
    ) {
        return memory;
    }

#if defined(_WIN32)
    if (previous_layout.alignment == new_layout.alignment) {
        return _aligned_realloc(memory, new_layout.size, new_layout.alignment);
    } else {
        void* previous_memory = memory;
        defer _aligned_free(previous_memory);

        memory = _aligned_alloc(new_layout.size, new_layout.alignment);
        memcpy(memory, previous_memory, previous_layout.size);
        return memory;
    }
#else
    void* previous_memory = memory;
    // defer free_aligned_sized(previous_memory, previous_layout.alignment, previous_layout.size);
    defer free(previous_memory);

    memory = aligned_alloc(new_layout.alignment, new_layout.size);
    memcpy(memory, previous_memory, previous_layout.size);
    return memory;
#endif
}


static void
_cndDefaultDeallocate(
    void* userdata,
    void* memory,
    CndAllocLayout layout
) {
    assert(memory != nullptr);
    assert(layout.size > 0);
    assert(layout.alignment > 0);
    assert(layout.size % layout.alignment == 0);

#if defined(_WIN32)
    _aligned_free(memory);
#else
    // free_aligned_sized(memory, layout.alignment, layout.size);
    free(memory);
#endif
}


CNDAPI_ATTR void CNDAPI_CALL
cndAllocatorCallbacksPopulateDefault(CndAllocatorCallbacks* allocator) {
    if (allocator == nullptr) return;
    *allocator = (CndAllocatorCallbacks) {
        .userdata = nullptr,
        .allocate = _cndDefaultAllocate,
        .reallocate = _cndDefaultReallocate,
        .deallocate = _cndDefaultDeallocate,
    };
}


CNDAPI_ATTR void* CNDAPI_CALL
cndAllocate(CndAllocatorCallbacks allocator, size_t size) {
    assert(allocator.allocate != nullptr);
    if (size == 0) {
        return nullptr;
    }
    auto layout = cndAllocLayout(size, 0);
    return allocator.allocate(allocator.userdata, layout);
}


CNDAPI_ATTR void* CNDAPI_CALL
cndAllocateAligned(CndAllocatorCallbacks allocator, size_t size, size_t alignment) {
    assert(allocator.allocate != nullptr);
    if (size == 0) {
        return nullptr;
    }
    auto layout = cndAllocLayout(size, alignment);
    return allocator.allocate(allocator.userdata, layout);
}


CNDAPI_ATTR void* CNDAPI_CALL
cndReallocate(CndAllocatorCallbacks allocator, void* memory, size_t previous_size, size_t new_size) {
    assert(allocator.reallocate != nullptr);
    if (new_size == 0) {
        cndDeallocate(allocator, memory, previous_size);
        return nullptr;
    } else if (memory == nullptr) {
        auto layout = cndAllocLayout(new_size, 0);
        return allocator.allocate(allocator.userdata, layout);
    } else {
        auto previous_layout = cndAllocLayout(previous_size, 0);
        auto new_layout = cndAllocLayout(new_size, 0);
        return allocator.reallocate(allocator.userdata, memory, previous_layout, new_layout);
    }
}


CNDAPI_ATTR void* CNDAPI_CALL
cndReallocateAligned(CndAllocatorCallbacks allocator, void* memory, size_t previous_size, size_t new_size, size_t alignment) {
    assert(allocator.allocate != nullptr);
    assert(allocator.reallocate != nullptr);
    if (new_size == 0) {
        cndDeallocateAligned(allocator, memory, previous_size, alignment);
        return nullptr;
    } else if (memory == nullptr) {
        auto layout = cndAllocLayout(new_size, alignment);
        return allocator.allocate(allocator.userdata, layout);
    } else {
        auto previous_layout = cndAllocLayout(previous_size, alignment);
        auto new_layout = cndAllocLayout(new_size, alignment);
        return allocator.reallocate(allocator.userdata, memory, previous_layout, new_layout);
    }
}


CNDAPI_ATTR void CNDAPI_CALL
cndDeallocate(CndAllocatorCallbacks allocator, void* memory, size_t size) {
    assert(allocator.deallocate != nullptr);
    if (memory == nullptr) return;
    auto layout = cndAllocLayout(size, 0);
    allocator.deallocate(allocator.userdata, memory, layout);
}


CNDAPI_ATTR void CNDAPI_CALL
cndDeallocateAligned(CndAllocatorCallbacks allocator, void* memory, size_t size, size_t alignment) {
    assert(allocator.deallocate != nullptr);
    if (memory == nullptr) return;
    auto layout = cndAllocLayout(size, alignment);
    allocator.deallocate(allocator.userdata, memory, layout);
}
