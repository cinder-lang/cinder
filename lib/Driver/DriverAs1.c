#include "../Support/String.h"

#include "Driver.h"


CndResult
_cndDriverParseAs1CLIArguments(
    CndDriverRef driver,
    const char** arguments,
    int32_t count
) {
    if (driver == nullptr) return CND_ERROR_INVALID_HANDLE;

    _cndDriverCopyCLIArguments(driver, arguments, count);
    const CndConstString* argv = driver->cli_args.data;
    size_t argc = driver->cli_args.count;

    #define _shift()  (argc == 0 ? (CndConstString){} : (argc--, *argv++))

    // The first CLI argument is always what the tool was invoked as,
    // even if invoked programatically.
    driver->invoked_as = _shift();

    #undef _shift

    return CND_SUCCESS;
}


CndResult
_cndDriverRunAs1(CndDriverRef driver) {
    if (driver == nullptr) return CND_ERROR_INVALID_HANDLE;
    return CND_SUCCESS;
}
