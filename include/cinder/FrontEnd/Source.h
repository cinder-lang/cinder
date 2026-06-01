#ifndef CINDER__FRONT_END__SOURCE_H
#define CINDER__FRONT_END__SOURCE_H


#include <cinder/Support/Interface.h>
#include <cinder/Support/String.h>
#include <cinder/Support/Result.h>

#include <cinder/FrontEnd/OpaqueTypes.h>

/* TODO(echoe): I don't like this, but so far I want the driver to be in charge of sources. */
#include <cinder/Driver/OpaqueTypes.h>

#include <stdint.h>


typedef struct {
    CndSourceRef source;
    int64_t offset, length;
} CndSourceSpan;


CNDAPI_ATTR CndResult CNDAPI_CALL cndGetSourceFromFile(
    CndDriverRef driver,
    CndConstString file_path,
    CndSourceRef* out_source
);

CNDAPI_ATTR CndResult CNDAPI_CALL cndGetSourceFromString(
    CndDriverRef driver,
    CndConstString name,
    CndConstString text,
    CndSourceRef* out_source
);

CNDAPI_ATTR CndConstString CNDAPI_CALL cndGetSourceName(
    CndSourceRef source
);

CNDAPI_ATTR CndConstString CNDAPI_CALL cndGetSourceText(
    CndSourceRef source
);


#endif /* CINDER__FRONT_END__SOURCE_H */
