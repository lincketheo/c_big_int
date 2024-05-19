#include "big_int.h"
#include "test_big_int.h"
#include <stdio.h>

int main()
{
  test_various_others();
  test_bi_init();
  test_bi_add_bi();

  return 0;
}
