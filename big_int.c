//
// Created by Theo Lincke on 4/1/24.
//
#include "big_int.h"
#include <stdio.h>
#include <stdarg.h>
#include <strings.h>

void bi_error(const char *msg_fmt, ...) {
    va_list args;
    va_start(args, msg_fmt);
    printf("c_big_int error: ");
    vprintf(msg_fmt, args);
}

/**
 * Utils
 */
void bi_init(struct big_int *bi, size_t initial_capacity) {
    if (!bi) {
        bi_error("bi_init failed because bi was uninitialized\n");
        return;
    }
    bi_clean(*bi);
    bi->data = malloc(initial_capacity);
    bi->capacity = initial_capacity;
    bi->size = 1;
    bzero(bi->data, initial_capacity);
}

void bi_clean(struct big_int bi) {
    if (bi.data) {
        free(bi.data);
    }
    bi.data = NULL;
}

void bi_print(struct big_int bi) {

}

void bi_print_hex(struct big_int bi) {
    printf("0x");
    for (int i = 0; i < bi.size; ++i) {
        uint8_t right = bi.data[bi.size - 1 - i] & 0xF;
        uint8_t left = (bi.data[bi.size - 1 - i] & 0xF0) >> 4;
        if (i == 0 && left == 0) {
            printf("%x", right);
        } else {
            printf("%x%x", left, right);
        }
    }
    printf("\n");
}

/**
 * Big Int X Bit Int
 */
void bi_add(struct big_int *dest, struct big_int left, struct big_int right) {
    if (!dest) {
        bi_error("bi_add, invalid dest\n");
        return;
    }
    size_t max_size = right.size > left.size ? right.size : left.size;
    if (dest->capacity < max_size + 1) {
        dest->capacity = max_size * 2;
        uint8_t* new_head;
        if(!(new_head = realloc(dest->data, dest->capacity))) {
            bi_error("bi_add, failed to realloc\n");
            return;
        }
        dest->data = new_head;
    }
    uint8_t k = 0; // carry
    uint16_t w; // sum value - note sum(ui + vi + k) <= (b - 1) + (b - 1) < 2b - b = 256
    for (int j = 0; j < right.size || j < left.size; ++j) {
        w = k;
        if (j < left.size) w += left.data[j];
        if (j < right.size) w += right.data[j];
        k = w / 256;
        w = w % 256;
        dest->data[j] = w;
    }
}

void bi_mult(struct big_int *dest, struct big_int left, struct big_int right) {

}

void bi_subtract(struct big_int *dest, struct big_int left, struct big_int right) {

}

void bi_div(struct big_int *dest, struct big_int left, struct big_int right) {

}

void bi_mod(struct big_int *dest, struct big_int left, struct big_int right) {

}

/**
 * Big Int X Int
 */
void bi_add_sc(struct big_int *dest, struct big_int left, int right) {

}

void bi_mult_sc(struct big_int *dest, struct big_int left, int right) {

}

void bi_subtract_sc_l(struct big_int *dest, struct big_int left, int right) {

}

void bi_div_sc_l(struct big_int *dest, struct big_int left, int right) {

}

void bi_mod_sc_l(struct big_int *dest, struct big_int left, int right) {

}

/**
 * Int X Big Int
 */
void bi_subtract_sc_r(struct big_int *dest, struct big_int left, int right) {

}

void bi_div_sc_r(struct big_int *dest, int left, struct big_int right) {

}

void bi_mod_sc_r(struct big_int *dest, int left, struct big_int right) {

}

