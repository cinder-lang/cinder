#ifndef CINDER_IMPL__SUPPORT__STRING_H
#define CINDER_IMPL__SUPPORT__STRING_H


#include <cinder/Support/String.h>

#include <stdcountof.h>


#define CND_CONST_STRING(String)  ((CndConstString) { .data = "" String "", .count = countof(String) - 1 })

#define CND_STRING_FORMAT(String)  (int) (String).count, (String).data


#endif /* CINDER_IMPL__SUPPORT__STRING_H */
