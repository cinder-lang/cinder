#include "../Support/DynamicArray.h"

#include "Source.h"

#include "../Driver/Driver.h"

#include <errno.h>
#include <stddefer.h>
#include <stdio.h>
#include <string.h>


#if defined(_WIN32)
/* <stdio.h> should already provide _fseeki64 and _ftelli64 */
#else
#  define _fseeki64  fseek
#  define _ftelli64  ftell
#endif


CNDAPI_ATTR CndResult CNDAPI_CALL
cndGetSourceFromFile(
    CndDriverRef driver,
    CndConstString file_path,
    CndSourceRef* out_source
) {
    if (driver == nullptr) return CND_ERROR_INVALID_HANDLE;
    if (out_source == nullptr) return CND_ERROR_NULL_OUT_PARAMETER;

    char* file_path_c = cndAllocate(driver->allocator, file_path.count + 1);
    if (file_path_c == nullptr) return CND_ERROR_OUT_OF_MEMORY;
    defer cndDeallocate(driver->allocator, file_path_c, file_path.count + 1);

    strncpy(file_path_c, file_path.data, file_path.count);

    errno = 0;
    FILE* f = fopen(file_path_c, "r");
    if (f == nullptr) {
        _cndDriverIssueErrorF(driver, "failed to load source file: %s", strerror(errno));
        return CND_ERROR_FILE_SYSTEM;
    }

    defer fclose(f);

    errno = 0;
    if (_fseeki64(f, 0, SEEK_END) != 0) {
        _cndDriverIssueErrorF(driver, "failed to load source file: %s", strerror(errno));
        return CND_ERROR_FILE_SYSTEM;
    }

    errno = 0;
    int64_t t = _ftelli64(f);
    if (t == -1L) {
        _cndDriverIssueErrorF(driver, "failed to load source file: %s", strerror(errno));
        return CND_ERROR_FILE_SYSTEM;
    }

    size_t file_text_length = (size_t) t;

    errno = 0;
    if (_fseeki64(f, 0, SEEK_SET) != 0) {
        _cndDriverIssueErrorF(driver, "failed to load source file: %s", strerror(errno));
        return CND_ERROR_FILE_SYSTEM;
    }

    char* file_text_buf = cndAllocate(driver->allocator, file_text_length + 1);
    if (file_text_buf == nullptr) return CND_ERROR_OUT_OF_MEMORY;

    errno = 0;
    size_t nread = fread(file_text_buf, 1, file_text_length, f);
    if (nread != file_text_length) {
        cndDeallocate(driver->allocator, file_text_buf, file_text_length + 1);
        _cndDriverIssueErrorF(driver, "failed to load source file: %s", strerror(errno));
        return CND_ERROR_FILE_SYSTEM;
    }

    file_text_buf[file_text_length] = 0;

    char* file_name_buf = cndAllocate(driver->allocator, file_path.count + 1);
    if (file_name_buf == nullptr) {
        cndDeallocate(driver->allocator, file_text_buf, file_text_length + 1);
        return CND_ERROR_OUT_OF_MEMORY;
    }

    strncpy(file_name_buf, file_path.data, file_path.count + 1);

    CndSourceRef source = cndAllocateAligned(driver->allocator, sizeof(*source), alignof_e(*source));
    if (source == nullptr) {
        cndDeallocate(driver->allocator, file_text_buf, file_text_length + 1);
        cndDeallocate(driver->allocator, file_name_buf, file_path.count + 1);
        return CND_ERROR_OUT_OF_MEMORY;
    }

    *source = (struct CndSource) {
        .name.data = file_name_buf,
        .name.count = file_path.count,
        .text.data = file_text_buf,
        .text.count = file_text_length,
    };

    cndDynArrPush(&driver->sources, source);
    return CND_SUCCESS;
}


CNDAPI_ATTR CndResult CNDAPI_CALL
cndGetSourceFromString(
    CndDriverRef driver,
    CndConstString name,
    CndConstString text,
    CndSourceRef* out_source
) {
    if (driver == nullptr) return CND_ERROR_INVALID_HANDLE;
    if (out_source == nullptr) return CND_ERROR_NULL_OUT_PARAMETER;

    char* file_name_buf = cndAllocate(driver->allocator, name.count + 1);
    if (file_name_buf == nullptr) {
        return CND_ERROR_OUT_OF_MEMORY;
    }

    char* file_text_buf = cndAllocate(driver->allocator, text.count + 1);
    if (file_text_buf == nullptr) {
        cndDeallocate(driver->allocator, file_name_buf, name.count + 1);
        return CND_ERROR_OUT_OF_MEMORY;
    }

    strncpy(file_name_buf, name.data, name.count + 1);
    strncpy(file_text_buf, text.data, text.count + 1);

    CndSourceRef source = cndAllocateAligned(driver->allocator, sizeof(*source), alignof_e(*source));
    if (source == nullptr) {
        cndDeallocate(driver->allocator, file_name_buf, name.count + 1);
        cndDeallocate(driver->allocator, file_text_buf, text.count + 1);
        return CND_ERROR_OUT_OF_MEMORY;
    }

    *source = (struct CndSource) {
        .name.data = file_name_buf,
        .name.count = name.count,
        .text.data = file_text_buf,
        .text.count = text.count,
    };

    cndDynArrPush(&driver->sources, source);
    return CND_SUCCESS;
}


CNDAPI_ATTR CndConstString CNDAPI_CALL
cndGetSourceName(
    CndSourceRef source
) {
    if (source == nullptr) return (CndConstString) {};
    return cndStringAsConst(source->name);
}


CNDAPI_ATTR CndConstString CNDAPI_CALL
cndGetSourceText(
    CndSourceRef source
) {
    if (source == nullptr) return (CndConstString) {};
    return cndStringAsConst(source->text);
}
