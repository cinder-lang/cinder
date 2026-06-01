#ifndef CINDER__FRONT_END__LANGUAGE_STANDARDS_H
#define CINDER__FRONT_END__LANGUAGE_STANDARDS_H


#include <cinder/Support/Interface.h>
#include <cinder/Support/String.h>

#include <cinder/FrontEnd/Language.h>
#include <cinder/FrontEnd/LanguageFeatures.h>

#include <stdbool.h>
#include <stdint.h>


typedef enum {
    CND_LANGSTD_UNSPECIFIED,
#define STD(Ident, Short, Lang, Desc, Flags, Ver) CND_LANGSTD_##Ident,
#include "XLanguageStandards.h"
} CndLanguageStandardKind;

typedef struct {
    CndLanguageStandardKind kind;
    CndLanguage language;
    CndConstString short_name;
    CndConstString description;
    CndLanguageFeatures features;
    int64_t version;
} CndLanguageStandard;


CNDAPI_ATTR CndLanguageStandardKind CNDAPI_CALL cndGetDefaultLanguageStandardKind(CndLanguage language);

CNDAPI_ATTR const CndLanguageStandard* CNDAPI_CALL cndGetLanguageStandardFromKind(CndLanguageStandardKind kind);
CNDAPI_ATTR const CndLanguageStandard* CNDAPI_CALL cndGetLanguageStandardFromShortName(CndConstString short_name);

CNDAPI_ATTR bool CNDAPI_CALL cndLanguageStandardHasHexFloats(const CndLanguageStandard* std);
CNDAPI_ATTR bool CNDAPI_CALL cndLanguageStandardHasDigraphs(const CndLanguageStandard* std);
CNDAPI_ATTR bool CNDAPI_CALL cndLanguageStandardHasLineComments(const CndLanguageStandard* std);
CNDAPI_ATTR bool CNDAPI_CALL cndLanguageStandardHasRawStringLiterals(const CndLanguageStandard* std);

CNDAPI_ATTR bool CNDAPI_CALL cndLanguageStandardHasClangExtensions(const CndLanguageStandard* std);
CNDAPI_ATTR bool CNDAPI_CALL cndLanguageStandardHasMicrosoftExtensions(const CndLanguageStandard* std);
CNDAPI_ATTR bool CNDAPI_CALL cndLanguageStandardHasGNUExtensions(const CndLanguageStandard* std);

CNDAPI_ATTR bool CNDAPI_CALL cndLanguageStandardIsC(const CndLanguageStandard* std);
CNDAPI_ATTR bool CNDAPI_CALL cndLanguageStandardIsC99(const CndLanguageStandard* std);
CNDAPI_ATTR bool CNDAPI_CALL cndLanguageStandardIsC11(const CndLanguageStandard* std);
CNDAPI_ATTR bool CNDAPI_CALL cndLanguageStandardIsC17(const CndLanguageStandard* std);
CNDAPI_ATTR bool CNDAPI_CALL cndLanguageStandardIsC23(const CndLanguageStandard* std);
CNDAPI_ATTR bool CNDAPI_CALL cndLanguageStandardIsC29(const CndLanguageStandard* std);

CNDAPI_ATTR bool CNDAPI_CALL cndLanguageStandardIsCinder(const CndLanguageStandard* std);


#endif /* CINDER__FRONT_END__LANGUAGE_STANDARDS_H */
