#ifndef CINDER__SUPPORT__ALLOC_H
#define CINDER__SUPPORT__ALLOC_H


#include <cinder/Support/Interface.h>
#include <cinder/Support/Align.h>

#include <stddef.h>


typedef struct CndAllocLayout {
    size_t size;
    size_t alignment;
} CndAllocLayout;

typedef void* (*CndFnAllocate)(void* userdata, CndAllocLayout layout);
typedef void* (*CndFnReallocate)(void* userdata, void* memory, CndAllocLayout previous_layout, CndAllocLayout new_layout);
typedef void  (*CndFnDeallocate)(void* userdata, void* memory, CndAllocLayout layout);

typedef struct CndAllocatorCallbacks {
    void* userdata;
    CndFnAllocate allocate;
    CndFnReallocate reallocate;
    CndFnDeallocate deallocate;
} CndAllocatorCallbacks;


CNDAPI_ATTR CndAllocLayout CNDAPI_CALL cndAllocLayout(size_t size, size_t alignment);


#endif /* CINDER__SUPPORT__ALLOC_H */
