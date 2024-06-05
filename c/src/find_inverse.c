#include "zp/zp.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Syntax: %s <x> <n>", argv[0]);
        exit(EXIT_FAILURE);
    }

    zp_t x = zp_new(strtoull(argv[1], NULL, 0));
    uint64_t n = strtoull(argv[2], NULL, 0);
    zp_t y = zp_inv(x, n);

    printf("1/%" PRIu64 " mod %" PRIu64 " = %" PRIu64 "\n", zp_as_int(x), n, zp_as_int(y));

    return 0;
}
