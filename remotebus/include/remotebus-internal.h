#include <config.h>

#ifndef REMOTEBUS_INTERNAL_H
#define REMOTEBUS_INTERNAL_H

#ifdef  __cplusplus
#  define RBUS_EXTERN_START  extern "C" {
#  define RBUS_EXTERN_END    }
#else
#  define RBUS_EXTERN_START  
#  define RBUS_EXTERN_END   
#endif

#ifndef TRUE
#  define TRUE 1
#endif
#ifndef FALSE
#  define FALSE 0
#endif

#ifndef NULL
#  ifdef __cplusplus
#    define NULL        (0L)
#  else 
#    define NULL        ((void*) 0)
#  endif 
#endif

RBUS_EXTERN_START

#if     __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ > 4)
#define _RBUS_GNUC_PRINTF( format_idx, arg_idx )    \
  __attribute__((__format__ (__printf__, format_idx, arg_idx)))
#define _RBUS_GNUC_NORETURN                         \
  __attribute__((__noreturn__))
#else   /* !__GNUC__ */
#define _RBUS_GNUC_PRINTF( format_idx, arg_idx )
#define _RBUS_GNUC_NORETURN
#endif  /* !__GNUC__ */

#if defined (__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#define _RBUS_FUNCTION_NAME __func__
#elif defined(__GNUC__) || defined(_MSC_VER)
#define _RBUS_FUNCTION_NAME __FUNCTION__
#else
#define _RBUS_FUNCTION_NAME "unknown function"
#endif


#ifdef RBUS_ENABLE_VERBOSE_MODE
void _rbus_verbose_real(const char *file, int line, const char *function_name, const char *format,...) _RBUS_GNUC_PRINTF (4, 5);
#define _rbus_verbose(format,args...) _rbus_verbose_real(__FILE__,__LINE__,_RBUS_FUNCTION_NAME,format,##args)
#else
static void _rbus_verbose(const char *x,...) {;}
#endif /* !RBUS_ENABLE_VERBOSE_MODE */

RBUS_EXTERN_END

#endif 
