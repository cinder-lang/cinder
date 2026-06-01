#ifndef CINDER_IMPL__SUPPORT__DYNAMIC_ARRAY_H
#define CINDER_IMPL__SUPPORT__DYNAMIC_ARRAY_H


#include <cinder/Support/Interface.h>

#include "Align.h"
#include "Alloc.h"

#include <assert.h>
#include <stdalign.h>


#define CNDIMPL_DYNAMIC_ARRAY_FIELDS(ElementType)  \
    CndAllocatorCallbacks allocator; ElementType* data; size_t count, capacity;

#define CNDIMPL_DYNAMIC_ARRAY(ElementType)  \
    struct { CNDIMPL_DYNAMIC_ARRAY_FIELDS(ElementType) }


static inline void
_cndDynArrReserveImpl(
    CndAllocatorCallbacks* allocator,
    void** data_ptr,
    size_t min_capacity,
    size_t* capacity_ptr,
    size_t element_size,
    size_t element_align
) {
    assert(allocator != nullptr);
    assert(data_ptr != nullptr);
    assert(capacity_ptr != nullptr);
    assert(element_size > 0);
    assert(element_align > 0);
    assert(element_size % element_align == 0);

    if (allocator->allocate == nullptr) {
        cndAllocatorCallbacksPopulateDefault(allocator);
    }

    void* data = *data_ptr;
    size_t previous_capacity = *capacity_ptr;

    size_t capacity = previous_capacity;
    if (capacity >= min_capacity) return;
    if (capacity == 0) capacity = 256;
    while (capacity < min_capacity) capacity *= 2;

    data = cndReallocateAligned(*allocator, data, previous_capacity * element_size, capacity * element_size, element_align);

    *data_ptr = data;
}


#define cndDynArrReserve(DynArr, MinCap)  do { \
        assert((DynArr) != nullptr); \
        _cndDynArrReserveImpl(&(DynArr)->allocator, (void**) &(DynArr)->data, (MinCap), &(DynArr)->capacity, sizeof(*(DynArr)->data), alignof_e(*(DynArr)->data)); \
    } while (0)

#define cndDynArrDealloc(DynArr)  do { \
        if ((DynArr)->data == nullptr) break; \
        assert((DynArr)->allocator.deallocate != nullptr); \
        cndDeallocateAligned((DynArr)->allocator, (DynArr)->data, (DynArr)->capacity * sizeof(*(DynArr)->data), alignof_e(*(DynArr)->data)); \
    } while (0)

#define cndDynArrPush(DynArr, Elem)  do { \
        cndDynArrReserve((DynArr), (DynArr)->count + 1); \
        (DynArr)->data[(DynArr)->count++] = (Elem); \
    } while (0)


#endif /* CINDER_IMPL__SUPPORT__DYNAMIC_ARRAY_H */
