#ifndef LABC_UTIL_H_
#define LABC_UTIL_H_


#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


// ----------------------------------------------------------------------------
// - Allocation Macros --------------------------------------------------------
// ----------------------------------------------------------------------------


#define newblock(T, size) ((T *) malloc(sizeof(T) * size))
#define new(T)            ((T *) newblock(T, 1))

#define delete(ptr) { \
    if (ptr) {        \
        free(ptr);    \
        ptr = NULL;   \
    }                 \
}


// ----------------------------------------------------------------------------
// - Type Decl Macros ---------------------------------------------------------
// ----------------------------------------------------------------------------


#define HAVE_DEFTYPE
#define deftype(T, ...) deftype_ ## T ## _t (__VA_ARGS__)

#define deftype_blockptr_t(name, size) typedef char (* name)[size]

#define deftype_Maybe_t(T) \
typedef struct {           \
    T    value;            \
    bool error;            \
} Maybe_ ## T


// ----------------------------------------------------------------------------
// - Cast Macros --------------------------------------------------------------
// ----------------------------------------------------------------------------


// Numerical casts
#define Char(n)    ((char) (n))
#define Short(n)   ((short) (n))
#define Int(n)     ((int) (n))
#define Long(n)    ((long) (n))
#define Int8(n)    ((int8_t) (n))
#define Int16(n)   ((int16_t) (n))
#define Int32(n)   ((int32_t) (n))
#define Int64(n)   ((int64_t) (n))
#define UInt8(n)   ((uint8_t) (n))
#define UInt16(n)  ((uint16_t) (n))
#define UInt32(n)  ((uint32_t) (n))
#define UInt64(n)  ((uint64_t) (n))
#define Float(n)   ((float) (n))
#define Double(n)  ((double) (n))
#define Ptr(p)     ((void*) (p))


#endif // !LABC_UTIL_H_
