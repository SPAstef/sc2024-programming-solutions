#include "zp/zp.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char **argv)
{
    if (argc < 4)
    {
        fprintf(stderr, "Syntax: %s <x> <y> <n>", argv[0]);
        exit(EXIT_FAILURE);
    }

    zp_t x = zp_new(strtoul(argv[1], NULL, 0));
    uint64_t y = strtoull(argv[2], NULL, 0);
    uint64_t n = strtoull(argv[3], NULL, 0);
    zp_t z = zp_pow(x, y, n);

    printf("%" PRIu64 " ^ %" PRIu64 " mod %" PRIu64 " = %" PRIu64 "\n", zp_as_int(x), y, n,
           zp_as_int(z));

    return 0;
}
