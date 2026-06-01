#ifndef CINDER__SUPPORT__STRING_H
#define CINDER__SUPPORT__STRING_H


#include <cinder/Support/Interface.h>
#include <cinder/Support/Arena.h>

#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>


typedef struct CndString {
    char* data;
    size_t count;
} CndString;

typedef struct CndConstString {
    const char* data;
    size_t count;
} CndConstString;


CNDAPI_ATTR CndString CNDAPI_CALL cndString(char* cstring);
CNDAPI_ATTR CndConstString CNDAPI_CALL cndConstString(const char* cstring);
CNDAPI_ATTR CndConstString CNDAPI_CALL cndConstStringN(const char* cstring, size_t count);

CNDAPI_ATTR CndConstString CNDAPI_CALL cndStringAsConst(CndString string);


CNDAPI_ATTR bool CNDAPI_CALL cndConstStringEquals(CndConstString a, CndConstString b);
CNDAPI_ATTR bool CNDAPI_CALL cndConstStringEqualsC(CndConstString a, const char* b);
CNDAPI_ATTR bool CNDAPI_CALL cndConstStringStartsWith(CndConstString string, CndConstString prefix);
CNDAPI_ATTR bool CNDAPI_CALL cndConstStringStartsWithC(CndConstString string, const char* prefix);
CNDAPI_ATTR CndConstString CNDAPI_CALL cndConstStringTrimPrefix(CndConstString string, CndConstString prefix);
CNDAPI_ATTR CndConstString CNDAPI_CALL cndConstStringTrimPrefixC(CndConstString string, const char* prefix);

CNDAPI_ATTR CndConstString CNDAPI_CALL cndConstStringGetFileExtension(CndConstString file_name);


CNDAPI_ATTR CndString CNDAPI_CALL cndArenaStringCopy(CndArenaRef arena, CndConstString string);
CNDAPI_ATTR CndString CNDAPI_CALL cndArenaStringCopyC(CndArenaRef arena, const char* cstring);
CNDAPI_ATTR CndString CNDAPI_CALL cndArenaStringFormat(CndArenaRef arena, const char* format, ...);
CNDAPI_ATTR CndString CNDAPI_CALL cndArenaStringFormatV(CndArenaRef arena, const char* format, va_list v);


#endif /* CINDER__SUPPORT__STRING_H */
