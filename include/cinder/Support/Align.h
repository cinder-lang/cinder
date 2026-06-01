#ifndef CINDER__SUPPORT__ALIGN_H
#define CINDER__SUPPORT__ALIGN_H


#include <cinder/Support/Interface.h>

#include <stdbool.h>
#include <stddef.h>


CNDAPI_ATTR size_t CNDAPI_CALL cndAlignPadding(size_t value, size_t alignment);
CNDAPI_ATTR size_t CNDAPI_CALL cndAlignTo(size_t value, size_t alignment);
CNDAPI_ATTR size_t CNDAPI_CALL cndAlignNormalize(size_t alignment);

CNDAPI_ATTR bool CNDAPI_CALL cndIsValueAlignedTo(size_t value, size_t alignment);
CNDAPI_ATTR bool CNDAPI_CALL cndIsPointerAlignedTo(void* pointer, size_t alignment);


#endif /* CINDER__SUPPORT__ALIGN_H */
