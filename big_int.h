//
// Created by Theo Lincke on 4/1/24.
//

#ifndef C_BIG_INT_BIG_INT_H
#define C_BIG_INT_BIG_INT_H

#include <stdint.h>
#include <stdlib.h>

/**
 * A Big Int is a stream of bits (uint8_t* array)
 * Each [bpd] bits is a "digit"
 */
struct big_uint {
  uint8_t* data;

  uint base;
  uint bpd; // bits per digit - derived from base

  size_t size;      // Size wrt exponent
  size_t capacity;  // Actual capacity of data
  size_t data_size; // Actual size of data
};

/**
 * Utils
 */
int bi_init(struct big_uint* bi, uint64_t start, uint64_t base);

void bi_free(struct big_uint* bi);

/**
 * Returns a string (null terminated and malloced!)
 * that shows the power representation. For example,
 * base = 7:
 * "5x7^4 + 3x7^3 + 2x7^2 + 1x7^1 + 0x7^0"
 */
char* bi_power_format(struct big_uint* bi);

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
