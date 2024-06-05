#include "fq/fq.h"
#include "fq/fq_vec.h"
#include "zp/zp_poly.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    if (argc < 5)
    {
        fprintf(stderr,
                "Syntax: %s <p> <k> <[[a_0_*, ..., a_0_0]; ...; [a_n_*, ..., a_n_0]]> "
                "<[[b_0_*, ..., b_0_0]; ...; [b_n_*, ..., b_n_0]]>",
                argv[0]);
        exit(EXIT_FAILURE);
    }

    uint64_t p = strtoull(argv[1], NULL, 0);
    size_t k = strtoull(argv[2], NULL, 0);
    zp_poly_t r = zp_poly_find_irred(p, k);

    printf("Found irreducible polynomial r = ");
    zp_poly_print(stdout, r);
    putchar('\n');

    fq_vec_t a = fq_vec_from_str(argv[3]);
    fq_vec_t b = fq_vec_from_str(argv[4]);
    fq_t c = fq_vec_dot(a, b, r, p);

    fq_vec_print(stdout, a);
    printf("\n*\n");
    fq_vec_print(stdout, b);
    printf("\n=\n");
    fq_print(stdout, c);
    printf(" (mod ");
    zp_poly_print(stdout, r);
    printf(")\n");

    zp_poly_del(r);
    fq_del(c);
    fq_vec_del(a);
    fq_vec_del(b);

    return 0;
}
