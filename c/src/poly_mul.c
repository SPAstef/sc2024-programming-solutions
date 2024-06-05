#include "zp/zp_poly.h"

#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc < 5)
    {
        fprintf(stderr, "Syntax: %s <p> <[a_n0, ..., a_0]> <[b_n1, ..., b_0]> <[c_n2, ..., c_0]>",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    uint64_t p = strtoul(argv[1], NULL, 0);
    zp_poly_t a = zp_poly_from_str(argv[2]);
    zp_poly_t b = zp_poly_from_str(argv[3]);
    zp_poly_t c = zp_poly_from_str(argv[4]);
    zp_poly_t t = zp_poly_mul(a, b, p);
    zp_poly_t r = zp_poly_rem(t, c, p);

    zp_poly_print(stdout, r);
    printf("\n");

    zp_poly_del(a);
    zp_poly_del(b);
    zp_poly_del(c);
    zp_poly_del(t);
    zp_poly_del(r);

    return 0;
}
