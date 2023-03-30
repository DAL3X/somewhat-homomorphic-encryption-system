#ifndef DATA
#define DATA

#include <stdint.h>
#include "../../util/parameters.hpp"

// Data in the form defined in https://eprint.iacr.org/archive/2013/866/1390397338.pdf.

typedef struct RLK_CONSTANTS
{
    uint64_t rlk00[NUMPRIMES][PACKED_POLY_DEG];
    uint64_t rlk01[NUMPRIMES][PACKED_POLY_DEG];
    uint64_t rlk10[NUMPRIMES][PACKED_POLY_DEG];
    uint64_t rlk11[NUMPRIMES][PACKED_POLY_DEG];
    uint64_t rlk20[NUMPRIMES][PACKED_POLY_DEG];
    uint64_t rlk21[NUMPRIMES][PACKED_POLY_DEG];
    uint64_t rlk30[NUMPRIMES][PACKED_POLY_DEG];
    uint64_t rlk31[NUMPRIMES][PACKED_POLY_DEG];
    uint64_t rlk40[NUMPRIMES][PACKED_POLY_DEG];
    uint64_t rlk41[NUMPRIMES][PACKED_POLY_DEG];
    uint64_t rlk50[NUMPRIMES][PACKED_POLY_DEG];
    uint64_t rlk51[NUMPRIMES][PACKED_POLY_DEG];
} RLK_CONSTANTS;

typedef struct IN_CIPHERTEXT
{
    uint64_t c00[NUMPRIMES][PACKED_POLY_DEG];
    uint64_t c01[NUMPRIMES][PACKED_POLY_DEG];
    uint64_t c10[NUMPRIMES][PACKED_POLY_DEG];
    uint64_t c11[NUMPRIMES][PACKED_POLY_DEG];
} IN_CIPHERTEXT;

typedef struct OUT_CIPHERTEXT
{
    uint64_t c0[NUMPRIMES][PACKED_POLY_DEG];
    uint64_t c1[NUMPRIMES][PACKED_POLY_DEG];
} OUT_CIPHERTEXT;

#endif