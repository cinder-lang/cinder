#include <cinder/FrontEnd/LanguageStandards.h>

#include "../Support/Align.h"
#include "../Support/Alloc.h"

#include "../FrontEnd/Source.h"

#include "Driver.h"


CNDAPI_ATTR CndDriverRef CNDAPI_CALL
cndDriverCreate(
    const CndDriverCreateArgs* args,
    CndAllocatorCallbacks* allocator
) {
    CndAllocatorCallbacks _allocator = {};
    if (allocator != nullptr) {
        _allocator = *allocator;
    } else {
        cndAllocatorCallbacksPopulateDefault(&_allocator);
    }

    CndDriverRef driver = cndAllocateAligned(_allocator, sizeof(*driver), alignof_e(*driver));
    *driver = (struct CndDriver) {
        .kind = args->kind,
        .allocator = _allocator,
        .cli_args.allocator = _allocator,
    };

    driver->string_arena = cndArenaCreate(16 * 1024, &_allocator);

    driver->cstd = cndGetLanguageStandardFromKind(cndGetDefaultLanguageStandardKind(CND_LANG_C));
    driver->cndstd = cndGetLanguageStandardFromKind(cndGetDefaultLanguageStandardKind(CND_LANG_CINDER));

    return driver;
}


CNDAPI_ATTR void CNDAPI_CALL
cndDriverDestroy(CndDriverRef driver) {
    if (driver == nullptr) return;

    auto allocator = driver->allocator;

    for (size_t i = 0; i < driver->sources.count; i++) {
        auto source = driver->sources.data[i];
        cndDeallocate(driver->allocator, source->name.data, source->name.count + 1);
        cndDeallocate(driver->allocator, source->text.data, source->text.count + 1);
        cndDeallocateAligned(driver->allocator, source, sizeof(*source), alignof_e(*source));
    }

    cndDynArrDealloc(&driver->cli_args);
    cndDynArrDealloc(&driver->input_files);
    cndDynArrDealloc(&driver->sources);

    cndArenaDestroy(driver->string_arena);

    cndDeallocateAligned(allocator, driver, sizeof(*driver), alignof_e(*driver));
}


CNDAPI_ATTR CndResult CNDAPI_CALL
cndDriverRun(
    CndDriverRef driver
) {
    if (driver == nullptr) return CND_ERROR_INVALID_HANDLE;
    switch (driver->kind) {
        default: return CND_ERROR_INVALID_DRIVER_KIND;
        case CND_DRIVER_HIGH_LEVEL: return _cndDriverRunHighLevel(driver);
        case CND_DRIVER_CNDC1: return _cndDriverRunCndC1(driver);
        case CND_DRIVER_CC1: return _cndDriverRunCC1(driver);
        case CND_DRIVER_AS1: return _cndDriverRunAs1(driver);
    }
}


CNDAPI_ATTR bool CNDAPI_CALL
cndDriverHasIssuedError(
    CndDriverRef driver
) {
    if (driver == nullptr) return false;
    return driver->has_issued_error;
}

CNDAPI_ATTR CndResult CNDAPI_CALL
cndDriverParseCLIArguments(
    CndDriverRef driver,
    const char** arguments,
    int32_t count
) {
    if (driver == nullptr) return CND_ERROR_INVALID_HANDLE;
    switch (driver->kind) {
        default: return CND_ERROR_INVALID_DRIVER_KIND;
        case CND_DRIVER_HIGH_LEVEL: return _cndDriverParseHighLevelCLIArguments(driver, arguments, count);
        case CND_DRIVER_CNDC1: return _cndDriverParseCndC1CLIArguments(driver, arguments, count);
        case CND_DRIVER_CC1: return _cndDriverParseCC1CLIArguments(driver, arguments, count);
        case CND_DRIVER_AS1: return _cndDriverParseAs1CLIArguments(driver, arguments, count);
    }
}


CNDAPI_ATTR CndConstString CNDAPI_CALL
cndDriverGetCLIInvokedAs(CndDriverRef driver) {
    if (driver == nullptr) return (CndConstString){};
    return driver->invoked_as;
}
