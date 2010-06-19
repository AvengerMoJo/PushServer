#ifndef REMOTEBUS_TYPE_H
#define REMOTEBUS_TYPE_H

#include <remotebus/include/remotebus-internal.h> 

RBUS_EXTERN_START

#define RBUS_MAJOR_VERSION 0
#define RBUS_MINOR_VERSION 0
#define RBUS_MICRO_VERSION 1
#define RBUS_VERSION_STRING "0.0.1"
#define RBUS_VERSION ((0 << 16) | (0 << 8) | (1)) 


#if @RBUS_HAVE_INT64@
#define RBUS_HAVE_INT64 1
typedef @RBUS_INT64_TYPE@ rbus_int64_t;
typedef unsigned @RBUS_INT64_TYPE@ rbus_uint64_t;

#define RBUS_INT64_CONSTANT(val)  (@RBUS_INT64_CONSTANT@)
#define RBUS_UINT64_CONSTANT(val) (@RBUS_UINT64_CONSTANT@)

#else
#undef RBUS_HAVE_INT64
#undef RBUS_INT64_CONSTANT
#undef RBUS_UINT64_CONSTANT
#endif

typedef @RBUS_INT32_TYPE@ rbus_int32_t;
typedef unsigned @RBUS_INT32_TYPE@ rbus_uint32_t;

typedef @RBUS_INT16_TYPE@ rbus_int16_t;
typedef unsigned @RBUS_INT16_TYPE@ rbus_uint16_t;

RBUS_EXTERN_END

#endif
