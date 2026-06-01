#ifndef CINDER__SUPPORT__ARENA_H
#define CINDER__SUPPORT__ARENA_H


#include <cinder/Support/Interface.h>
#include <cinder/Support/Alloc.h>
#include <cinder/Support/OpaqueTypes.h>


CNDAPI_ATTR CndArenaRef CNDAPI_CALL cndArenaCreate(size_t block_size, CndAllocatorCallbacks* allocator);
CNDAPI_ATTR void CNDAPI_CALL cndArenaDestroy(CndArenaRef arena);

CNDAPI_ATTR void* CNDAPI_CALL cndArenaAlloc(CndArenaRef arena, size_t size);
CNDAPI_ATTR void* CNDAPI_CALL cndArenaAllocAligned(CndArenaRef arena, size_t size, size_t alignment);
CNDAPI_ATTR void  CNDAPI_CALL cndArenaClear(CndArenaRef arena);


#endif /* CINDER__SUPPORT__ARENA_H */
