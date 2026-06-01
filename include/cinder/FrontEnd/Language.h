#ifndef CINDER__FRONT_END__LANGUAGE_H
#define CINDER__FRONT_END__LANGUAGE_H


#include <cinder/Support/Interface.h>
#include <cinder/Support/String.h>


typedef enum {
    CND_LANG_UNSPECIFIED,
    CND_LANG_CINDER,
    CND_LANG_C,
} CndLanguage;


CNDAPI_ATTR CndConstString CNDAPI_CALL cndLanguageToString(CndLanguage language);


#endif /* CINDER__FRONT_END__LANGUAGE_H */
