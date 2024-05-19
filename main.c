#include <stdio.h>
#include "big_int.h"

int main() {
  printf("Hello, World!\n");
  struct big_uint bi;
  struct big_uint bi_2;
  bi_init(&bi, 10, 10);
  bi_init(&bi_2, 10, 10);
  bi_add_bi(&bi, &bi_2);
  bi_print(&bi);
  bi_add_bi(&bi, &bi_2);
  bi_print(&bi);
  bi_add_bi(&bi, &bi_2);
  bi_print(&bi);
  bi_add_bi(&bi, &bi_2);
  bi_print(&bi);
  bi_add_bi(&bi, &bi_2);
  bi_print(&bi);
  bi_add_bi(&bi, &bi_2);
  bi_print(&bi);
  bi_add_bi(&bi, &bi_2);
  bi_print(&bi);
  bi_add_bi(&bi, &bi_2);
  bi_print(&bi);
  bi_add_bi(&bi, &bi_2);
  bi_print(&bi);
  bi_add_bi(&bi, &bi_2);
  bi_print(&bi);
  bi_add_bi(&bi, &bi_2);
  bi_print(&bi);
  return 0;
}
