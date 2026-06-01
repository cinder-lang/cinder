#include "Align.h"

#include <stdint.h>


CNDAPI_ATTR size_t CNDAPI_CALL
cndAlignPadding(size_t value, size_t alignment) {
    return (alignment - (value % alignment)) % alignment;
}


CNDAPI_ATTR size_t CNDAPI_CALL
cndAlignTo(size_t value, size_t alignment) {
    return value + cndAlignPadding(value, alignment);
}


CNDAPI_ATTR size_t CNDAPI_CALL
cndAlignNormalize(size_t alignment) {
    if (alignment == 0) return 1;
    alignment--;
    alignment |= alignment >> 1;
    alignment |= alignment >> 2;
    alignment |= alignment >> 4;
    alignment |= alignment >> 8;
    alignment |= alignment >> 16;
    alignment |= alignment >> 32;
    alignment++;
    return alignment;
}


CNDAPI_ATTR bool CNDAPI_CALL
cndIsValueAlignedTo(size_t value, size_t alignment) {
    return 0 == (value % alignment);
}


CNDAPI_ATTR bool CNDAPI_CALL
cndIsPointerAlignedTo(void* pointer, size_t alignment) {
    return 0 == (((uintptr_t) pointer) % alignment);
}
