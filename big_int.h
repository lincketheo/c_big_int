//
// Created by Theo Lincke on 4/1/24.
//

#ifndef C_BIG_INT_BIG_INT_H
#define C_BIG_INT_BIG_INT_H

#include <stdint.h>
#include <stdlib.h>

/**
 * Base is MAX_UINT64_t / 2 because of carry in add
 */
#define MAX_BASE (0x7FFFFFFFFFFFFFFF)

enum span {
  UI8 = 1,
  UI16 = 2,
  UI32 = 4,
  UI64 = 8
};

struct big_uint {
  uint8_t* data; 

  size_t size;          // Number of elements (respects [span])
  size_t capacity;      // Number of bytes in data (doesn't respect [span])

  const uint64_t base;  
  const size_t span;    
};

/**
 * Utils
 */
int bi_init(
    struct big_uint* bi, 
    uint64_t start, 
    uint64_t base);

void bi_free(struct big_uint* bi);

/**
 * Returns a string (null terminated and malloced!)
 * that shows the power representation. For example,
 * base = 7:
 * "5x7^4 + 3x7^3 + 2x7^2 + 1x7^1 + 0x7^0"
 */
void bi_print(const struct big_uint* bi);

/**
 * Big Int += Bit Int
 */
int bi_add_bi(
    struct big_uint* dest,
    const struct big_uint* right);

/**
 * Big Int += Scalar
 */
int bi_add_sc(
    struct big_uint* dest,
    const uint64_t right);

#endif // C_BIG_INT_BIG_INT_H
