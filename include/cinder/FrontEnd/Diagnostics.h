#ifndef CINDER__FRONT_END__DIAGNOSTICS_H
#define CINDER__FRONT_END__DIAGNOSTICS_H


#include <cinder/Support/Interface.h>
#include <cinder/Support/Alloc.h>
#include <cinder/Support/Result.h>

#include <cinder/FrontEnd/OpaqueTypes.h>
#include <cinder/FrontEnd/Source.h>


typedef enum {
    CND_DIAG_GROUP_DEFAULT,
    CND_DIAG_GROUP_ALL,
} CndDiagnosticGroup;

typedef enum {
    CND_DIAG_LEVEL_IGNORED,
    CND_DIAG_LEVEL_NOTE,
    CND_DIAG_LEVEL_WARNING,
    CND_DIAG_LEVEL_ERROR,
    CND_DIAG_LEVEL_FATAL_ERROR,
} CndDiagnosticLevel;

typedef enum {
    CND_DIAG_SEMANTIC_NOTE,
    CND_DIAG_SEMANTIC_WARNING,
    CND_DIAG_SEMANTIC_EXTENSION,
    CND_DIAG_SEMANTIC_EXTENSION_WARNING,
    CND_DIAG_SEMANTIC_ERROR,
    CND_DIAG_SEMANTIC_FATAL_ERROR,
} CndDiagnosticSemantic;

typedef enum {
#define DIAG(Ident, Semantic, FlagName, GroupName) CND_DIAG_##Ident,
#include "XDiagnostics.h"
} CndDiagnostic;

typedef void (*CndFnDiagnosticCallback)(
    void* userdata,
    CndDiagnostic diag,
    CndDiagnosticLevel level,
    CndSourceSpan span,
    CndConstString message
);


#endif /* CINDER__FRONT_END__DIAGNOSTICS_H */
