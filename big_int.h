//
// Created by Theo Lincke on 4/1/24.
//

#ifndef C_BIG_INT_BIG_INT_H
#define C_BIG_INT_BIG_INT_H

#include <stdlib.h>

struct big_int {
    uint8_t *data;
    size_t size;
    size_t capacity;
};

/**
 * Utils
 */
void bi_init(struct big_int *bi, size_t initial_capacity);

void bi_clean(struct big_int bi);

void bi_print(struct big_int bi);

void bi_print_hex(struct big_int bi);

/**
 * Big Int X Bit Int
 */
void bi_add(struct big_int *dest, struct big_int left, struct big_int right);

void bi_mult(struct big_int *dest, struct big_int left, struct big_int right);

void bi_subtract(struct big_int *dest, struct big_int left, struct big_int right);

void bi_div(struct big_int *dest, struct big_int left, struct big_int right);

void bi_mod(struct big_int *dest, struct big_int left, struct big_int right);

/**
 * Big Int X Int
 */
void bi_add_sc(struct big_int *dest, struct big_int left, int right);

void bi_mult_sc(struct big_int *dest, struct big_int left, int right);

void bi_subtract_sc_l(struct big_int *dest, struct big_int left, int right);

void bi_div_sc_l(struct big_int *dest, struct big_int left, int right);

void bi_mod_sc_l(struct big_int *dest, struct big_int left, int right);

/**
 * Int X Big Int
 */
void bi_subtract_sc_r(struct big_int *dest, struct big_int left, int right);

void bi_div_sc_r(struct big_int *dest, int left, struct big_int right);

void bi_mod_sc_r(struct big_int *dest, int left, struct big_int right);

#endif //C_BIG_INT_BIG_INT_H
