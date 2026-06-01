#include <cinder/FrontEnd/LanguageStandards.h>

#include "../Support/String.h"

#include <assert.h>
#include <stddef.h>


static CndLanguageStandard _cnd_language_standards[] = {
#define STD(Ident, Short, Lang, Desc, Flags, Ver)  [CND_LANGSTD_##Ident] = { \
        .kind = CND_LANGSTD_##Ident, \
        .language = CND_LANG_##Lang, \
        .short_name = CND_CONST_STRING(Short), \
        .description = CND_CONST_STRING(Desc), \
        .features = (Flags), \
        .version = (Ver), \
    },
#include <cinder/FrontEnd/XLanguageStandards.h>
};

static struct {
    CndConstString alias;
    CndLanguageStandardKind std_kind;
} _cnd_language_standard_aliases[] = {
#define STD_ALIAS(Ident, Short)  { .alias = CND_CONST_STRING(Short), .std_kind = CND_LANGSTD_##Ident, },
#define STD_DEPR(Ident, Short)  STD_ALIAS(Ident, Short)
#define STD(Ident, Short, Lang, Desc, Flags, Ver)  STD_ALIAS(Ident, Short)
#include <cinder/FrontEnd/XLanguageStandards.h>
};


CNDAPI_ATTR CndLanguageStandardKind CNDAPI_CALL
cndGetDefaultLanguageStandardKind(CndLanguage language) {
    switch (language) {
        default:
        case CND_LANG_UNSPECIFIED: return CND_LANGSTD_UNSPECIFIED;
        case CND_LANG_CINDER: return CND_LANGSTD_CND2x;
        case CND_LANG_C: return CND_LANGSTD_C29;
    }
}


CNDAPI_ATTR const CndLanguageStandard* CNDAPI_CALL
cndGetLanguageStandardFromKind(CndLanguageStandardKind kind) {
    if (kind < CND_LANGSTD_UNSPECIFIED || kind >= countof(_cnd_language_standards)) {
        return nullptr;
    }

    return &_cnd_language_standards[kind];
}


CNDAPI_ATTR const CndLanguageStandard* CNDAPI_CALL
cndGetLanguageStandardFromShortName(CndConstString short_name) {
    for (size_t i = 0; i < countof(_cnd_language_standard_aliases); i++) {
        auto info = _cnd_language_standard_aliases[i];
        if (cndConstStringEquals(short_name, info.alias)) {
            assert(info.std_kind > CND_LANGSTD_UNSPECIFIED && info.std_kind < countof(_cnd_language_standards));
            return &_cnd_language_standards[info.std_kind];
        }
    }

    return nullptr;
}


CNDAPI_ATTR bool CNDAPI_CALL
cndLanguageStandardHasHexFloats(const CndLanguageStandard* std) {
    if (std == nullptr) return false;
    return 0 != (std->features & CND_LANGFEAT_HEXFLOAT);
}


CNDAPI_ATTR bool CNDAPI_CALL
cndLanguageStandardHasDigraphs(const CndLanguageStandard* std) {
    if (std == nullptr) return false;
    return 0 != (std->features & CND_LANGFEAT_DIGRAPHS);
}


CNDAPI_ATTR bool CNDAPI_CALL
cndLanguageStandardHasLineComments(const CndLanguageStandard* std) {
    if (std == nullptr) return false;
    return 0 != (std->features & CND_LANGFEAT_LINE_COMMENT);
}


CNDAPI_ATTR bool CNDAPI_CALL
cndLanguageStandardHasRawStringLiterals(const CndLanguageStandard* std) {
    if (std == nullptr) return false;
    /* GNU C has raw string literals since C99. */
    return cndLanguageStandardHasGNUExtensions(std) &&
           cndLanguageStandardIsC99(std);
}


CNDAPI_ATTR bool CNDAPI_CALL
cndLanguageStandardHasClangExtensions(const CndLanguageStandard* std) {
    if (std == nullptr) return false;
    return 0 != (std->features & CND_LANGFEAT_CLANG);
}


CNDAPI_ATTR bool CNDAPI_CALL
cndLanguageStandardHasMicrosoftExtensions(const CndLanguageStandard* std) {
    if (std == nullptr) return false;
    return 0 != (std->features & CND_LANGFEAT_MSVC);
}


CNDAPI_ATTR bool CNDAPI_CALL
cndLanguageStandardHasGNUExtensions(const CndLanguageStandard* std) {
    if (std == nullptr) return false;
    return 0 != (std->features & CND_LANGFEAT_GNU);
}


CNDAPI_ATTR bool CNDAPI_CALL
cndLanguageStandardIsC(const CndLanguageStandard* std) {
    if (std == nullptr) return false;
    return 0 != (std->features & CND_LANGFEAT_C);
}


CNDAPI_ATTR bool CNDAPI_CALL
cndLanguageStandardIsC99(const CndLanguageStandard* std) {
    if (std == nullptr) return false;
    return 0 != (std->features & CND_LANGFEAT_C99);
}


CNDAPI_ATTR bool CNDAPI_CALL
cndLanguageStandardIsC11(const CndLanguageStandard* std) {
    if (std == nullptr) return false;
    return 0 != (std->features & CND_LANGFEAT_C11);
}


CNDAPI_ATTR bool CNDAPI_CALL
cndLanguageStandardIsC17(const CndLanguageStandard* std) {
    if (std == nullptr) return false;
    return 0 != (std->features & CND_LANGFEAT_C17);
}


CNDAPI_ATTR bool CNDAPI_CALL
cndLanguageStandardIsC23(const CndLanguageStandard* std) {
    if (std == nullptr) return false;
    return 0 != (std->features & CND_LANGFEAT_C23);
}


CNDAPI_ATTR bool CNDAPI_CALL
cndLanguageStandardIsC29(const CndLanguageStandard* std) {
    if (std == nullptr) return false;
    return 0 != (std->features & CND_LANGFEAT_C29);
}


CNDAPI_ATTR bool CNDAPI_CALL
cndLanguageStandardIsCinder(const CndLanguageStandard* std) {
    if (std == nullptr) return false;
    return 0 != (std->features & CND_LANGFEAT_CND);
}
