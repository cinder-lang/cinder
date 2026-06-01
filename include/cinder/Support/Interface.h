#ifndef CINDER__SUPPORT__INTERFACE_H
#define CINDER__SUPPORT__INTERFACE_H


#if defined(_WIN32)
#  define CNDAPI_ATTR
#  define CNDAPI_CALL  __stdcall
#  define CNDAPI_FPTR  CNDAPI_CALL
#else
#  define CNDAPI_ATTR
#  define CNDAPI_CALL
#  define CNDAPI_FPTR
#endif


#if defined(__STDC_VERSION__) && (__STDC_VERSION__ - 0) >= 202000L
#  define CNDAPI_VOID
#else
#  define CNDAPI_VOID  void
#endif


#endif /* CINDER__SUPPORT__INTERFACE_H */
