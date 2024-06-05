#include "fq/fq.h"
#include "zp/zp_poly.h"
#include <inttypes.h>
#include <stdlib.h>
#include "intrinsics.h"

int main(int argc, char **argv)
{
    if (argc < 5)
    {
        fprintf(stderr, "Usage: %s <p> <[r_k, ..., r_0]> <[a_*, ..., a_0]> <b>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    uint64_t p = strtoull(argv[1], NULL, 0);
    zp_poly_t r = zp_poly_from_str(argv[2]);
    fq_t a = fq_from_str(argv[3]);
    uint64_t b = strtoull(argv[4], NULL, 0);

    uint64_t start = _rdtsc();
    fq_t x = fq_pow(a, b, r, p);
    uint64_t end = _rdtsc();

    printf("(");
    fq_print(stdout, a);
    printf(")^%" PRIu64 " = ", b);
    fq_print(stdout, x);
    printf("\n");
    printf("Time: %" PRIu64 " cycles\n", end - start);


    return 0;
}
