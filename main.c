#include <stdio.h>
#include "big_int.h"

int main() {
    printf("Hello, World!\n");
    struct big_int a;
    bi_init(&a, 10);
    a.data[0] = 5;
    a.data[1] = 3;
    a.data[2] = 4;
    a.size = 3;
    bi_print_hex(a);
    return 0;
}
