#include "fq/fq.h"
#include "utils.h"
#include "zp/zp_poly.h"
#include <inttypes.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc < 4)
    {
        fprintf(stderr, "Syntax: %s <x> <p> <k>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    uint64_t ix = strtoull(argv[1], NULL, 0);
    uint64_t p = strtoull(argv[2], NULL, 0);
    uint64_t k = strtoull(argv[3], NULL, 0);
    uint64_t q = pow64(p, k);
    zp_poly_t r = zp_poly_find_irred(p, k);

    printf("Found irreducible polynomial r = ");
    zp_poly_print(stdout, r);
    putchar('\n');

    fq_t x = fq_from_int(ix, p);
    printf("x = ");
    fq_print(stdout, x);
    putchar('\n');

    for (uint64_t i = 0; i <= q; ++i)
    {
        fq_t y = fq_pow(x, i, r, p);

        printf("x^%" PRIu64 " = ", i);
        fq_print(stdout, y);
        putchar('\n');

        fq_del(y);
    }

    fq_del(x);
    zp_poly_del(r);

    return 0;
}
