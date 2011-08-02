#ifndef REMOTEBUS_TYPE_H
#define REMOTEBUS_TYPE_H

#include <include/remotebus-internal.h> 

RBUS_EXTERN_START

#define RBUS_MAJOR_VERSION 0
#define RBUS_MINOR_VERSION 0
#define RBUS_MICRO_VERSION 2
#define RBUS_VERSION_STRING "0.0.2"
#define RBUS_VERSION ((0 << 16) | (0 << 8) | (2)) 


#if 1
#define RBUS_HAVE_INT64 1
typedef long rbus_int64_t;
typedef unsigned long rbus_uint64_t;

#define RBUS_INT64_CONSTANT(val)  ((val##L))
#define RBUS_UINT64_CONSTANT(val) ((val##UL))

#else
#undef RBUS_HAVE_INT64
#undef RBUS_INT64_CONSTANT
#undef RBUS_UINT64_CONSTANT
#endif

typedef int rbus_int32_t;
typedef unsigned int rbus_uint32_t;

typedef short rbus_int16_t;
typedef unsigned short rbus_uint16_t;

#ifndef DBUS_HAVE_INT64
/**
 * An 8-byte struct you could use to access int64 without having
 * int64 support
 */
typedef struct
{
  rbus_uint32_t first32;  /**< first 32 bits in the 8 bytes (beware endian issues) */
  rbus_uint32_t second32; /**< second 32 bits in the 8 bytes (beware endian issues) */
} RBus8ByteStruct;
#endif /* DBUS_HAVE_INT64 */

/**
 * A simple 8-byte value union that lets you access 8 bytes as if they
 * were various types; useful when dealing with basic types via
 * void pointers and varargs.
 */
typedef union
{
  rbus_int16_t  i16;   /**< as int16 */
  rbus_uint16_t u16;   /**< as int16 */
  rbus_int32_t  i32;   /**< as int32 */
  rbus_uint32_t u32;   /**< as int32 */
#ifdef DBUS_HAVE_INT64
  rbus_int64_t  i64;   /**< as int64 */
  rbus_uint64_t u64;   /**< as int64 */
#else
  RBus8ByteStruct u64; /**< as 8-byte-struct */
#endif
  double dbl;          /**< as double */
  unsigned char byt;   /**< as byte */
  char *str;           /**< as char* */
} RBusBasicValue;

RBUS_EXTERN_END

#endif
