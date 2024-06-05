#include "zp/zp_vec.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "Syntax: %s <p> [v_1, ..., v_n]", argv[0]);
        exit(EXIT_FAILURE);
    }

    uint64_t p = strtoull(argv[1], NULL, 0);
    zp_vec_t v = zp_vec_from_str(argv[2]);
    zp_vec_t w = zp_vec_circmul(v, v, p);

    printf("circ(");
    zp_vec_print(stdout, v);
    printf(") * (");
    zp_vec_print(stdout, v);
    printf(") = ");

    zp_vec_print(stdout, w);
    printf(" (mod %" PRIu64 ")\n", p);

    zp_vec_del(v);
    zp_vec_del(w);

    return 0;
}
