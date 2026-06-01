#ifndef CINDER_IMPL__SUPPORT__ALLOC_H
#define CINDER_IMPL__SUPPORT__ALLOC_H


#include <cinder/Support/Alloc.h>


CNDAPI_ATTR void CNDAPI_CALL cndAllocatorCallbacksPopulateDefault(CndAllocatorCallbacks* allocator);

CNDAPI_ATTR void* CNDAPI_CALL cndAllocate(CndAllocatorCallbacks allocator, size_t size);
CNDAPI_ATTR void* CNDAPI_CALL cndAllocateAligned(CndAllocatorCallbacks allocator, size_t size, size_t alignment);

CNDAPI_ATTR void* CNDAPI_CALL cndReallocate(CndAllocatorCallbacks allocator, void* memory, size_t previous_size, size_t new_size);
CNDAPI_ATTR void* CNDAPI_CALL cndReallocateAligned(CndAllocatorCallbacks allocator, void* memory, size_t previous_size, size_t new_size, size_t alignment);

CNDAPI_ATTR void  CNDAPI_CALL cndDeallocate(CndAllocatorCallbacks allocator, void* memory, size_t size);
CNDAPI_ATTR void  CNDAPI_CALL cndDeallocateAligned(CndAllocatorCallbacks allocator, void* memory, size_t size, size_t alignment);


#endif /* CINDER_IMPL__SUPPORT__ALLOC_H */
