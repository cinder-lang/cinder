#include <cinder/Support/Interface.h>

#include "String.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>


CNDAPI_ATTR CndString CNDAPI_CALL
cndString(char* cstring) {
    if (cstring == nullptr) return (CndString) {};
    return (CndString) {
        .data = cstring,
        .count = strlen(cstring),
    };
}


CNDAPI_ATTR CndConstString CNDAPI_CALL
cndConstString(const char* cstring) {
    if (cstring == nullptr) return (CndConstString) {};
    return (CndConstString) {
        .data = cstring,
        .count = strlen(cstring),
    };
}


CNDAPI_ATTR CndConstString CNDAPI_CALL
cndConstStringN(const char* cstring, size_t count) {
    return (CndConstString) {
        .data = cstring,
        .count = count,
    };
}


CNDAPI_ATTR CndConstString CNDAPI_CALL
cndStringAsConst(CndString string) {
    return (CndConstString) {
        .data = string.data,
        .count = string.count,
    };
}


CNDAPI_ATTR bool CNDAPI_CALL
cndConstStringEquals(CndConstString a, CndConstString b) {
    if (a.count != b.count) return false;
    return 0 == strncmp(a.data, b.data, a.count);
}


CNDAPI_ATTR bool CNDAPI_CALL
cndConstStringEqualsC(CndConstString string, const char* prefix) {
    return cndConstStringEquals(string, cndConstString(prefix));
}


CNDAPI_ATTR bool CNDAPI_CALL
cndConstStringStartsWith(CndConstString string, CndConstString prefix) {
    if (prefix.count > string.count) return false;
    return 0 == strncmp(string.data, prefix.data, prefix.count);
}


CNDAPI_ATTR bool CNDAPI_CALL
cndConstStringStartsWithC(CndConstString string, const char* prefix) {
    return cndConstStringStartsWith(string, cndConstString(prefix));
}


CNDAPI_ATTR CndConstString CNDAPI_CALL
cndConstStringTrimPrefix(CndConstString string, CndConstString prefix) {
    if (prefix.count > string.count) {
        return string;
    }

    if (0 != strncmp(string.data, prefix.data, prefix.count)) {
        return string;
    }

    return (CndConstString) {
        .data = string.data + prefix.count,
        .count = string.count - prefix.count,
    };
}


CNDAPI_ATTR CndConstString CNDAPI_CALL
cndConstStringTrimPrefixC(CndConstString string, const char* prefix) {
    return cndConstStringTrimPrefix(string, cndConstString(prefix));
}


CNDAPI_ATTR CndConstString CNDAPI_CALL
cndConstStringGetFileExtension(CndConstString file_name) {
    if (file_name.count == 0) return (CndConstString) {};

    const char* begin = file_name.data;
    const char* end = begin + file_name.count;

    const char* file_name_begin = end;
    for (const char* seek = file_name_begin; seek >= begin; seek--) {
        if (*seek == '/' || *seek == '\\') break;
        file_name_begin = seek;
    }

    const char* extension_begin = end;
    for (const char* seek = extension_begin; seek >= begin; seek--) {
        if (*seek == '.') {
            extension_begin = seek;
            break;
        }
    }

    return (CndConstString) {
        .data = extension_begin,
        .count = end - extension_begin,
    };
}


CNDAPI_ATTR CndString CNDAPI_CALL
cndArenaStringCopy(CndArenaRef arena, CndConstString string) {
    if (arena == nullptr || string.count == 0) {
        return (CndString) {};
    }

    char* buf = cndArenaAlloc(arena, string.count + 1);
    (void) strncpy(buf, string.data, string.count + 1);

    return (CndString) {
        .data = buf,
        .count = string.count,
    };
}


CNDAPI_ATTR CndString CNDAPI_CALL
cndArenaStringCopyC(CndArenaRef arena, const char* cstring) {
    CndConstString string = cndConstString(cstring);
    return cndArenaStringCopy(arena, string);
}


CNDAPI_ATTR CndString CNDAPI_CALL
cndArenaStringFormat(CndArenaRef arena, const char* format, ...) {
    va_list v;
    va_start(v);
    CndString result = cndArenaStringFormatV(arena, format, v);
    va_end(v);
    return result;
}


CNDAPI_ATTR CndString CNDAPI_CALL
cndArenaStringFormatV(CndArenaRef arena, const char* format, va_list v) {
    if (arena == nullptr) return (CndString) {};

    va_list v1;
    va_copy(v1, v);
    int n = vsnprintf(nullptr, 0, format, v1);
    va_end(v1);

    size_t buf_length = (size_t) n + 1;
    char* buf = cndArenaAlloc(arena, buf_length);

    (void) vsnprintf(buf, buf_length, format, v);

    return (CndString) {
        .data = buf,
        .count = buf_length - 1,
    };
}
