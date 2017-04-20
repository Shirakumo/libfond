#ifndef __LIBFOND_H__
#define __LIBFOND_H__
#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
#  ifdef FOND_STATIC_DEFINE
#    define FOND_EXPORT
#  else
#    ifndef FOND_EXPORT
#      define FOND_EXPORT __declspec(dllexport)
#    endif
#  endif
#  if _MSC_VER <= 1600
#    define bool int
#    define true 1
#    define false 0
#  else
#    include <stdbool.h>
#  endif
#else
#  define FOND_EXPORT
#  include <stdbool.h>
#endif

#ifdef __cplusplus
}
#endif
#endif
