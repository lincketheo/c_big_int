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
#include <inttypes.h>

static uint quick_pow10(uint8_t n);
static void bi_error(const char* msg_fmt, ...);
static void bi_test_passed(const char* msg_fmt, ...);
static void bi_test_failed(const char* msg_fmt, ...);
static size_t calc_strlen_bi_power_format(const struct big_uint* bi);
static uint min_bits_needed(const uint num);
static uint min_decs_needed(const uint num);
static int double_capacity(struct big_uint* bi);
static char* bits8_str( uint8_t x );
static char* bits64_str( uint64_t x );
static uint64_t get_bits_span(
  const uint8_t* data, 
  const size_t dl, 
  const size_t elem_bit_size,
  const size_t i
);
static void set_bits_span(
  uint8_t* data, 
  const size_t dl, 
  const size_t elem_bit_size,
  const size_t i,
  uint64_t val
);

/**
 * Utils
 */
int bi_init(
  struct big_uint* bi, 
  uint64_t start, 
  uint64_t base)
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
  bi->size = 1;
  bi->bpd = min_bits_needed(base - 1); // for base 7, the maximum value is 6
  bi->base = base;

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

char* bi_bits(const struct big_uint* bi) {
  size_t bits_l = bi->size * bi->bpd;
  char* ret = malloc(bits_l);
  if(!ret)
    return NULL;
  size_t bit_i = 0;
  for(int i = 0; i < bi->size; ++i) {
    uint64_t elem_i = get_bits_span(bi->data, bi->capacity, bi->bpd, i);
    for(int j = 0; j < bi->bpd; ++j) {
      ret[bit_i] = (elem_i >> j) & 1 ? '1' : '0';
      bit_i++;
    }
  }
  return ret;
}

char* bi_power_format(const struct big_uint* bi)
{
  size_t len = calc_strlen_bi_power_format(bi);
  char* ret = malloc(len + 1);
  ret[len] = '\0';
  return ret;
}

int bi_add_bi(
    struct big_uint* dest,
    const struct big_uint* right)
{
  if (dest->base != right->base) {
    bi_error("Can only add two big ints if they have the same base\n");
    return -1;
  }

  uint64_t carry = 0; // 0 or 1
  size_t max_size = dest->size > right->size ? dest->size : right->size;

  // Max Size is biggest size plus one (if there's a carry at the end)
  size_t necessary_capacity = ((max_size + 1) * dest->bpd) / 8 + 1;
  if(dest->capacity < necessary_capacity)
    double_capacity(dest);

  for (int i = 0; i < max_size; ++i) {

    uint64_t sum = carry; 

    if(i < dest->size)
      sum += get_bits_span(dest->data, dest->capacity, dest->bpd, i);
    if(i < right->size)
      sum += get_bits_span(right->data, right->capacity, right->bpd, i);
    if(sum >= right->base){
      carry = 1;
      sum -= right->base;
    }

    set_bits_span(dest->data, dest->capacity, dest->bpd, i, sum);
  }

  dest->size = max_size;
  return 0;
}

int test_bi_add_bi()
{
  struct big_uint left;
  struct big_uint right;
  int ret = 0;

  bi_init(&left, 10, 10);
  bi_init(&right, 10, 2);
  if (!bi_add_bi(&left, &right)) {
    bi_test_failed("bi_add_bi different bases\n");
    ret = -1;
  } else {
    bi_test_passed("bi_add_bi different bases\n");
  }
  bi_free(&left);
  bi_free(&right);

  bi_init(&left, 10, 10);
  bi_init(&right, 10, 10);
  if (bi_add_bi(&left, &right)) {
    bi_test_failed("bi_add_bi (10 + 10)\n");
    ret = -1;
  } else {
    bi_test_passed("bi_add_bi (10 + 10)\n");
  }
  bi_free(&left);
  bi_free(&right);

  return ret;
}

/**
 * Big Int += Scalar
 */
int bi_add_sc(
    struct big_uint* dest,
    const uint64_t right)
{
  return -1;
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

static size_t calc_strlen_bi_power_format(const struct big_uint* bi)
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
  return -1;
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
  return -1;
}

static char* bits8_str( uint8_t x )
{
  char* ret = malloc(9);
  if(!ret){
    bi_error("Failed to allocate memory for bits8_str");
    return NULL;
  }
  for (int i = 8 - 1; i >= 0; --i)
    ret[8 - i - 1] = x & (1u << i) ? '1' : '0';
  ret[8] = '\0';
  return ret;
}

static int test_bits8_str_once(uint8_t val, char* expected) {
  char* actual = bits8_str(val);
  if(!actual) {
    bi_test_failed("Breaking early\n");
    return -1;
  }

  int ret = 0;
  if(strcmp(actual, expected)){
    bi_test_failed("bits8_str(%d) Expected: %s, Actual: %s\n", val, expected, actual);
    ret = -1;
  }
  bi_test_passed("bits8_str(%d)\n", val);
  free(actual);
  return ret;
}

static int test_bits8_str() {
  int ret = 0;
  ret = test_bits8_str_once(1, "00000001") | ret;
  ret = test_bits8_str_once(2, "00000010") | ret;
  ret = test_bits8_str_once(4, "00000100") | ret;
  ret = test_bits8_str_once(8, "00001000") | ret;
  ret = test_bits8_str_once(7, "00000111") | ret;
  ret = test_bits8_str_once(255, "11111111") | ret;
  ret = test_bits8_str_once(17, "00010001") | ret;
  ret = test_bits8_str_once(20, "00010100") | ret;
  return -ret;
}

static char* bits64_str( uint64_t x )
{
  char* ret = malloc(65);
  int i=0;
  for (i = 64 - 1; i >= 0; --i)
    ret[64 - i - 1] = x & (1ul << i) ? '1' : '0';
  ret[64] = '\0';
  return ret;
}

static int test_bits64_str_once(uint64_t val, char* expected) {
  char* actual = bits64_str(val);
  if(!actual) {
    bi_test_failed("Breaking early\n");
    return -1;
  }

  int ret = 0;
  if(strcmp(actual, expected)){
    bi_test_failed("bits64_str(%d) Expected: %s, Actual: %s\n", val, expected, actual);
    ret = -1;
  }
  bi_test_passed("bits64_str(%" PRIu64 ")\n", val);
  free(actual);
  return ret;
}

static int test_bits64_str() {
  int ret = 0;
  ret = test_bits64_str_once(1u, "0000000000000000000000000000000000000000000000000000000000000001") | ret;
  ret = test_bits64_str_once(2u, "0000000000000000000000000000000000000000000000000000000000000010") | ret;
  ret = test_bits64_str_once(4u, "0000000000000000000000000000000000000000000000000000000000000100") | ret;
  ret = test_bits64_str_once(8u, "0000000000000000000000000000000000000000000000000000000000001000") | ret;
  ret = test_bits64_str_once(7u, "0000000000000000000000000000000000000000000000000000000000000111") | ret;
  ret = test_bits64_str_once(255u, "0000000000000000000000000000000000000000000000000000000011111111") | ret;
  ret = test_bits64_str_once(17u, "0000000000000000000000000000000000000000000000000000000000010001") | ret;
  ret = test_bits64_str_once(20u, "0000000000000000000000000000000000000000000000000000000000010100") | ret;
  ret = test_bits64_str_once(11420900, "0000000000000000000000000000000000000000101011100100010011100100") | ret;
  ret = test_bits64_str_once((uint64_t)-1, "1111111111111111111111111111111111111111111111111111111111111111") | ret;
  return -ret;
}

static uint64_t get_bits(
  const uint8_t* data, 
  const size_t dl, 
  const size_t start, 
  const size_t end) {

  assert(start < end);
  assert(end - start <= 64);

  size_t bi = start;
  uint64_t ret = 0;
  uint64_t mask;

  while(end / 8 != bi / 8) {
    mask = data[dl - bi / 8 - 1]; // Get the whole byte

    // need 2 seperate ops because shift by negative is undefined
    mask >>= (bi % 8); // Only the top left 
    mask <<= (bi - start); // Move to return index

    ret |= mask;
    bi += 8 - (bi % 8); // Add remainder - only > 0 for first one
  }

  mask = ~(~0lu << (end % 8)) & (~0lu << (bi % 8)); // 1's from bi to end
  mask &= data[dl - end / 8 - 1]; 

  // need 2 seperate ops because shift by negative is undefined
  mask >>= bi % 8;
  mask <<= (bi - start);

  return ret | mask;
}

static uint64_t get_bits_span(
  const uint8_t* data, 
  const size_t dl, 
  const size_t elem_bit_size,
  const size_t i
){
  return get_bits(data, dl, elem_bit_size * i, elem_bit_size * (i + 1));
}

static int test_get_bits_once(uint8_t* data, size_t dl, size_t start, size_t end, uint64_t expected) {
  uint64_t actual = get_bits(data, dl, start, end);
  if(actual != expected){
    char* exp_s = bits64_str(expected);
    char* act_s = bits64_str(actual);
    bi_test_failed("Expected: %s Got: %s\n", exp_s, act_s);
    free(exp_s);
    free(act_s);
    return -1;
  }
  bi_test_passed("get_bits(data, %zu, %zu, %zu) == %" PRIu64 "\n", dl, start, end, expected);
  return 0;
}

static int test_get_bits() {
  size_t dl;
  int ret = 0;

  // 111111110100001100001000
  uint8_t case1[] = {255, 67, 8};
  dl = sizeof(case1);
  ret = ret | test_get_bits_once(case1, dl, 3, 20, 0b11110100001100001);
  ret = ret | test_get_bits_once(case1, dl, 3, 4, 0b1);
  ret = ret | test_get_bits_once(case1, dl, 8, 10, 0b11);
  ret = ret | test_get_bits_once(case1, dl, 9, 10, 0b1);
  ret = ret | test_get_bits_once(case1, dl, 9, 24, 0b111111110100001);

  return -ret;
}

static void set_bits(
  uint8_t* data, 
  const size_t dl, 
  const size_t start, 
  const size_t end, 
  uint64_t val) {
  assert(start < end);
  assert(end - start <= 64);
  
  // val can't take up more bits than end - start
  uint64_t vval = val & ~(~0ul << (end - start));
  assert(vval == val);
  
  size_t bi = start;

  while(end / 8 != bi / 8) {
    uint8_t zero = 0;
    uint8_t byte = data[dl - bi / 8 - 1]; 
    uint8_t right_ones = ~(~zero << (bi % 8));
    uint8_t data_right = byte & right_ones;
    uint8_t mask_left = (val >> (bi - start)) << (bi % 8); 

    data[dl - bi / 8 - 1] = data_right | mask_left;

    bi += 8 - (bi % 8); // Add remainder - only > 0 for first one
  }

  uint8_t byte = data[dl - bi / 8 - 1]; 

  uint8_t data_left = byte & 0xff << (8 - end % 8);
  uint8_t mask_right = val >> (bi - start);
  uint8_t data_right = byte & ~(~0 << (bi % 8));

  data[dl - bi / 8 - 1] = data_left | mask_right | data_right;
}

static void set_bits_span(
  uint8_t* data, 
  const size_t dl, 
  const size_t elem_bit_size,
  const size_t i,
  uint64_t val
){
  set_bits(data, dl, elem_bit_size * i, elem_bit_size * (i + 1), val);
}

static void print_bits(uint8_t* data, size_t dl, char* delim, char* byte_delim) {
  for(int i = 0; i < dl; ++i){
    char* str = bits8_str(data[i]);
    if(!str)
      return;
    for(int j = 0; j < 8; ++j)
      if(!((j + 1) % 8))
        printf("%c%s", str[j], byte_delim);
      else
        printf("%c%s", str[j], delim);
    free(str);
  }
}

/**
  * Tests that all the bits are the same. Indexes start at the far right 
  * (last data element, right most bit)
  * @return -1 if not and print 0 else
  */
static int test_same_bits(
  const uint8_t* expected, 
  const uint8_t* actual, 
  size_t dl, 
  size_t start, 
  size_t end) {

  size_t bi = 0;
  int ret = 0;
  for(int i = 0; i < dl; ++i) {
    uint8_t byte_e = expected[dl - i - 1];
    uint8_t byte_a = actual[dl - i - 1];

    for(int j = 0; j < 8; ++j){
      uint8_t bit_e = byte_e & (1 << j);
      uint8_t bit_a = byte_a & (1 << j);
      if(bi >= start && bi < end) {
        if(bit_e != bit_a){
          printf("Bit %zu does not match\n", bi);
          ret = -1;
        }
      }
      bi++;
    }
  }
  return ret;
}

static int test_set_bits_once(
  const uint8_t* _data, 
  size_t dl, 
  size_t start, 
  size_t end, 
  uint64_t expected, 
  int verbose) {
  uint8_t* data = malloc(dl);
  memcpy(data, _data, dl);

  // Print information
  if(verbose){
    // Not checking NULL mallocs
    printf(" Before: ");
    print_bits(data, dl, " ", "|");
    printf("\nSetting: ");
    for(int i = 0; i < dl * 8 - end; ++i)
      printf("  ");
    char* str = bits64_str(expected);
    for(int i = 0; i < (end - start); ++i) 
      printf("%c ", str[i + (64 - (end - start))]);
    printf("\n");
    free(str);
  }

  // Do work
  set_bits(data, dl, start, end, expected);

  // Print after information
  if(verbose){
    printf("  After: ");
    print_bits(data, dl, " ", "|");
    printf("\n");
  }

  int ret = 0;

  // Check that data remain unchanged at the far right
  if(test_same_bits(_data, data, dl, 0, start)) {
    printf("Far right failed\n");
    ret = -1;
  }

  // Check that data remain unchanged at the far left
  if(test_same_bits(_data, data, dl, end, dl * 8)) {
    printf("Far left failed\n");
    ret = -1;
  }

  // Check that data was correctly set
  uint64_t actual;
  if((actual = get_bits(data, dl, start, end)) != expected){
    char* exp_s = bits64_str(expected);
    char* act_s = bits64_str(actual);
    printf("Set data expected: %s Got: %s\n", exp_s, act_s);
    free(exp_s);
    free(act_s);
    ret = -1;
  }

  if(!ret)
    bi_test_passed("set_bits(data, %zu, %zu, %zu) == %" PRIu64 "\n", dl, start, end, expected);
  else
    bi_test_failed("set_bits(%" PRIu64 ")\n", expected);
  free(data);
  return 0;
}

static int test_set_bits(int verbose) {
  size_t dl;
  int ret = 0;

  // 111111110100001100001000
  uint8_t case1[] = {255, 67, 8};
  dl = sizeof(case1);
  ret = ret | test_set_bits_once(case1, dl, 3, 20, 0b10101100101, verbose);
  ret = ret | test_set_bits_once(case1, dl, 3, 4, 0b0, verbose);
  ret = ret | test_set_bits_once(case1, dl, 8, 10, 0b01, verbose);
  ret = ret | test_set_bits_once(case1, dl, 9, 10, 0b0, verbose);
  ret = ret | test_set_bits_once(case1, dl, 9, 24, 0b10110, verbose);

  return -ret;
}

int test_various_others()
{
  int ret = 0;
  ret = test_min_bits_needed() || ret;
  ret = test_min_decs_needed() || ret;
  ret = test_bits8_str() || ret;
  ret = test_bits64_str() || ret;
  ret = test_get_bits() || ret;
  ret = test_set_bits(0) || ret;
  return -ret;
}

