#include <cinder/Support/Interface.h>
#include <cinder/Support/String.h>

#include <cinder/FrontEnd/Language.h>

#include "../Support/String.h"

#include <stddef.h>

CNDAPI_ATTR CndConstString CNDAPI_CALL
cndLanguageToString(CndLanguage language) {
    switch (language)  {
        default: return CND_CONST_STRING("Unknown");
        case CND_LANG_CINDER: return CND_CONST_STRING("Cinder");
        case CND_LANG_C: return CND_CONST_STRING("C");
    }
}
