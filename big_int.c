//
// Created by Theo Lincke on 4/1/24.
//
#include "big_int.h"
#include "test_big_int.h"
#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <strings.h>

static uint quick_pow10(uint8_t n);
static void bi_error(const char* msg_fmt, ...);
static void bi_test_passed(const char* msg_fmt, ...);
static void bi_test_failed(const char* msg_fmt, ...);
static size_t calc_strlen_bi_power_format(struct big_uint* bi);
static uint min_bits_needed(const uint num);
static uint min_decs_needed(const uint num);
static int double_capacity(struct big_uint* bi);

/**
 * Utils
 */
int bi_init(struct big_uint* bi, uint64_t start, uint64_t base)
{
  if (!bi) {
    bi_error("bi_init failed because bi was uninitialized\n");
    return -1;
  }
  if (base == 0) {
    bi_error("bi_init failed because base was 0\n");
    return -1;
  }

  size_t start_cap = 1;
  bi->data = calloc(start_cap, 1);
  bi->capacity = start_cap;
  bi->data_size = 1;
  bi->size = 1;
  bi->bpd = min_bits_needed(base - 1); // for base 7, the maximum value is 6

  bi_add_sc(bi, start);

  return 0;
}

int test_bi_init()
{
  struct big_uint bi;
  int ret = 0;
  if (!bi_init(&bi, 0, 0)) {
    bi_test_failed("bi_init(0)\n");
    ret = -1;
  } else {
    bi_test_passed("bi_init(0)\n");
  }
  return ret;
}

void bi_free(struct big_uint* bi)
{
  if (bi->data)
    free(bi->data);
  bi->data = NULL;
}

char* bi_power_format(struct big_uint* bi)
{
  size_t len = calc_strlen_bi_power_format(bi);
  char* ret = malloc(len + 1);
  ret[len] = '\0';
  return ret;
}

static uint8_t top_left_most_1s(int n)
{
  assert(n > 0);
  assert(n < 8);
}

// Unsafe
static uint get_digit(const struct big_uint* bi, uint index)
{
  assert(bi->size > index);
  uint bit_start = index * bi->bpd;
  uint bit_end = bit_start + bi->bpd;
  uint bit_start_byte = bit_start / 8;
  uint bit_end_byte = bit_end / 8;

  uint ret = 0;

  // Get the upper part of the starting byte
  ret |= bit_start % 8;

  while (bit_start_byte <= bit_end_byte) {

    bit_start_byte++;
  }
}

int bi_add_bi(
    struct big_uint* dest,
    const struct big_uint* right)
{
  if (dest->base != right->base) {
    bi_error("Can only add two big ints if they have the same base\n");
    return -1;
  }

  uint carry = 0;
  uint max_size = dest->size > right->size ? dest->size : right->size;
  for (int i = 0; i < max_size; ++i) {
    uint dest_i = carry;
  }
}

int test_bi_add_bi()
{
  struct big_uint left;
  struct big_uint right;
  int ret = 0;

  bi_init(&left, 10, 1);
  bi_init(&left, 10, 2);
  if (!bi_add_bi(&left, &right)) {
    bi_test_failed("bi_add_bi different bases\n");
    ret = -1;
  }

  return ret;
}

/**
 * Big Int += Scalar
 */
int bi_add_sc(
    struct big_uint* dest,
    const uint64_t right)
{
}

/////////////////////////////////////// UTILS
static uint quick_pow10(uint8_t n)
{
  // TODO - make this better
  return (uint)powf(10, n);
}

static void bi_error(const char* msg_fmt, ...)
{
  va_list args;
  va_start(args, msg_fmt);
  printf("c_big_uint error: ");
  vprintf(msg_fmt, args);
}

static void bi_test_passed(const char* msg_fmt, ...)
{
  va_list args;
  va_start(args, msg_fmt);
  printf("c_big_int \033[1;32mTEST PASSED:\033[0m ");
  vprintf(msg_fmt, args);
}

static void bi_test_failed(const char* msg_fmt, ...)
{
  va_list args;
  va_start(args, msg_fmt);
  printf("c_big_int \033[1;31mTEST FAILED:\033[0m ");
  vprintf(msg_fmt, args);
}

static size_t calc_strlen_bi_power_format(struct big_uint* bi)
{
  uint prefix = min_decs_needed(bi->base - 1);
  uint exp_base = min_decs_needed(bi->base);
  uint exp = min_decs_needed(bi->size - 1);
  uint extra = 4; // "..x.. + " - minus the last one
  uint unit_len = prefix + exp_base + exp + extra;
  return unit_len * bi->size - extra;
}

static int test_calc_strlen_bi_power_format(struct big_uint* bi)
{
}

static uint min_bits_needed(const uint num)
{
  uint count = 0;
  while (num >> ++count)
    ;
  return count;
}

static int test_min_bits_needed_once(uint digit, uint expected)
{
  uint actual = min_bits_needed(digit);
  if (actual != expected) {
    bi_test_failed("min_bits_needed(%d) "
                   "Expected: %d Actual: %d\n",
        digit, expected, actual);
    return -1;
  }
  bi_test_passed("min_bits_needed(%d)\n", digit);
  return 0;
}

static int test_min_bits_needed()
{
  int ret = 0;
  ret = test_min_bits_needed_once(0, 1) || ret;
  ret = test_min_bits_needed_once(1, 1) || ret;
  ret = test_min_bits_needed_once(2, 2) || ret;
  ret = test_min_bits_needed_once(3, 2) || ret;
  ret = test_min_bits_needed_once(4, 3) || ret;
  ret = test_min_bits_needed_once(8, 4) || ret;
  ret = test_min_bits_needed_once(9999999, 24) || ret;
  return -ret;
}

static uint min_decs_needed(const uint num)
{
  uint count = 0;
  while (num / quick_pow10(++count))
    ;
  return count;
}

static int test_min_decs_needed_once(uint digit, uint expected)
{
  uint actual = min_decs_needed(digit);
  if (actual != expected) {
    bi_test_failed("min_decs_needed(%d) "
                   "Expected: %d Actual: %d\n",
        digit, expected, actual);
    return -1;
  }
  bi_test_passed("min_decs_needed(%d)\n", digit);
  return 0;
}

static int test_min_decs_needed()
{
  int ret = 0;
  ret = test_min_decs_needed_once(0, 1) || ret;
  ret = test_min_decs_needed_once(1, 1) || ret;
  ret = test_min_decs_needed_once(9, 1) || ret;
  ret = test_min_decs_needed_once(10, 2) || ret;
  ret = test_min_decs_needed_once(11, 2) || ret;
  ret = test_min_decs_needed_once(110, 3) || ret;
  ret = test_min_decs_needed_once(10234918, 8) || ret;
  return -ret;
}

static int double_capacity(struct big_uint* bi)
{
  size_t new_capacity = 2 * bi->capacity;
  uint8_t* new_head = realloc(bi->data, new_capacity);
  if (!new_head) {
    bi_error("Couldn't reallocate data when doubling capacity\n");
    return -1;
  }
  bi->data = new_head;
  bi->capacity = new_capacity;
  return 0;
}

static int test_double_capacity()
{
  // TODO
}

int test_various_others()
{
  int ret = 0;
  ret = test_min_bits_needed() || ret;
  ret = test_min_decs_needed() || ret;
  return -ret;
}
