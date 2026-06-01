#ifndef CINDER__DRIVER__DRIVER_H
#define CINDER__DRIVER__DRIVER_H


#include <cinder/Support/Interface.h>
#include <cinder/Support/Alloc.h>
#include <cinder/Support/Result.h>
#include <cinder/Support/String.h>

#include <cinder/FrontEnd/Diagnostics.h>

#include <cinder/Driver/OpaqueTypes.h>

#include <stdint.h>


typedef enum {
    CND_DRIVER_HIGH_LEVEL,
    CND_DRIVER_CNDC1,
    CND_DRIVER_CC1,
    CND_DRIVER_AS1,
} CndDriverKind;

typedef struct {
    CndDriverKind kind;
    CndFnDiagnosticCallback diagnostic_callback;
} CndDriverCreateArgs;


CNDAPI_ATTR CndDriverRef CNDAPI_CALL cndDriverCreate(
    const CndDriverCreateArgs* args,
    CndAllocatorCallbacks* allocator
);

CNDAPI_ATTR void CNDAPI_CALL cndDriverDestroy(
    CndDriverRef driver
);

CNDAPI_ATTR CndResult CNDAPI_CALL cndDriverRun(
    CndDriverRef driver
);


CNDAPI_ATTR bool CNDAPI_CALL cndDriverHasIssuedError(
    CndDriverRef driver
);


CNDAPI_ATTR CndResult CNDAPI_CALL cndDriverParseCLIArguments(
    CndDriverRef driver,
    const char** arguments,
    int32_t count
);

CNDAPI_ATTR CndConstString CNDAPI_CALL cndDriverGetCLIInvokedAs(
    CndDriverRef driver
);


#endif /* CINDER__DRIVER__DRIVER_H */
