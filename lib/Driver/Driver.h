#ifndef CINDER_IMPL__DRIVER__DRIVER_H
#define CINDER_IMPL__DRIVER__DRIVER_H


#include <cinder/Support/String.h>
#include <cinder/Support/Arena.h>

#include <cinder/FrontEnd/LanguageStandards.h>

#include <cinder/Driver/Driver.h>

#include "../Support/DynamicArray.h"
#include "../Support/String.h"

#include <stdarg.h>
#include <stdio.h>


typedef enum {
    CND_INFILE_UNSPECIFIED,
    CND_INFILE_CINDER_SOURCE,
    CND_INFILE_C_SOURCE,
    CND_INFILE_C_SOURCE_NOPP,
    CND_INFILE_C_HEADER,
} CndDriverInputFileKind;

typedef struct {
    CndDriverInputFileKind kind;
    CndConstString path;
} CndDriverInputFile;

typedef CNDIMPL_DYNAMIC_ARRAY(CndConstString) CndDriverCLIArgumentArray;

typedef CNDIMPL_DYNAMIC_ARRAY(CndSourceRef) CndSourceRefArray;

typedef CNDIMPL_DYNAMIC_ARRAY(CndDriverInputFile) CndDriverInputFileArray;

struct CndDriver {
    CndDriverKind kind;

    CndAllocatorCallbacks allocator;
    CndArenaRef string_arena;

    CndDriverCLIArgumentArray cli_args;

    CndConstString invoked_as;

    bool has_issued_error : 1;
    bool show_help : 1;
    bool show_version : 1;
    bool list_standards : 1;
    bool verbose : 1;
    bool hash3 : 1;

    const CndLanguageStandard* cstd;
    const CndLanguageStandard* cndstd;

    CndConstString output_file_path;
    CndDriverInputFileArray input_files;

    CndSourceRefArray sources;
};


static inline void
_cndDriverCopyCLIArguments(
    CndDriverRef driver,
    const char** arguments,
    int32_t count
) {
    for (int32_t i = 0; i < count; i++) {
        CndString arg = cndArenaStringCopyC(driver->string_arena, arguments[i]);
        // Trim out empty arguments
        if (arg.count == 0) continue;
        assert(arg.data != nullptr);
        cndDynArrPush(&driver->cli_args, cndStringAsConst(arg));
    }
}


CndResult _cndDriverParseHighLevelCLIArguments(CndDriverRef driver, const char** arguments, int32_t count);
CndResult _cndDriverParseCndC1CLIArguments(CndDriverRef driver, const char** arguments, int32_t count);
CndResult _cndDriverParseCC1CLIArguments(CndDriverRef driver, const char** arguments, int32_t count);
CndResult _cndDriverParseAs1CLIArguments(CndDriverRef driver, const char** arguments, int32_t count);

CndResult _cndDriverRunHighLevel(CndDriverRef driver);
CndResult _cndDriverRunCndC1(CndDriverRef driver);
CndResult _cndDriverRunCC1(CndDriverRef driver);
CndResult _cndDriverRunAs1(CndDriverRef driver);


static inline void
_cndVerbose(CndDriverRef driver, const char* message) {
    if (!driver->verbose) return;
    fprintf(stderr, "%s\n", message);
}


static inline void
_cndVerboseV(CndDriverRef driver, const char* format, va_list v) {
    if (!driver->verbose) return;
    vfprintf(stderr, format, v);
    fprintf(stderr, "\n");
}


static inline void
_cndVerboseF(CndDriverRef driver, const char* format, ...) {
    va_list v;
    va_start(v);
    _cndVerboseV(driver, format, v);
    va_end(v);
}


static inline void
_cndDriverIssueNote(CndDriverRef driver, const char* message) {
    fprintf(stderr, "%.*s: note: %s\n", CND_STRING_FORMAT(driver->invoked_as), message);
}


static inline void
_cndDriverIssueNoteV(CndDriverRef driver, const char* format, va_list v) {
    fprintf(stderr, "%.*s: note: ", CND_STRING_FORMAT(driver->invoked_as));
    vfprintf(stderr, format, v);
    fprintf(stderr, "\n");
}


static inline void
_cndDriverIssueNoteF(CndDriverRef driver, const char* format, ...) {
    va_list v;
    va_start(v);
    _cndDriverIssueNoteV(driver, format, v);
    va_end(v);
}


static inline void
_cndDriverIssueError(CndDriverRef driver, const char* message) {
    driver->has_issued_error = true;
    fprintf(stderr, "%.*s: error: %s\n", CND_STRING_FORMAT(driver->invoked_as), message);
}


static inline void
_cndDriverIssueErrorV(CndDriverRef driver, const char* format, va_list v) {
    driver->has_issued_error = true;
    fprintf(stderr, "%.*s: error: ", CND_STRING_FORMAT(driver->invoked_as));
    vfprintf(stderr, format, v);
    fprintf(stderr, "\n");
}


static inline void
_cndDriverIssueErrorF(CndDriverRef driver, const char* format, ...) {
    va_list v;
    va_start(v);
    _cndDriverIssueErrorV(driver, format, v);
    va_end(v);
}


#endif /* CINDER_IMPL__DRIVER__DRIVER_H */
