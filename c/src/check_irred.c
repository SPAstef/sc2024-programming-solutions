#include "zp/zp_poly.h"

#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Syntax: %s <p> <[a_*, ..., a_0]>", argv[0]);
        exit(EXIT_FAILURE);
    }

    uint64_t p = strtoul(argv[1], NULL, 0);

    zp_poly_t a = zp_poly_from_str(argv[2]);

    zp_poly_print(stdout, a);

    puts(zp_poly_is_irred(a, p) ? ": irreducible" : ": reducible");

    zp_poly_del(a);

    return 0;
}
