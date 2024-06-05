#include "string_utils.h"
#include "utils.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        printf("Usage: %s <hex_x> <hex_y>\n", argv[0]);
        return 1;
    }

    size_t nx = (strlen(argv[1]) + 1) / 2;
    size_t ny = (strlen(argv[2]) + 1) / 2;
    size_t n = max(nx, ny);
    uint8_t *x = malloc(n);
    uint8_t *y = malloc(n);
    uint8_t *z = malloc(n);


    hexload(x, n, argv[1]);
    hexload(y, n, argv[2]);

    for (size_t i = 0; i < n; i++)
        z[i] = x[i] ^ y[i];

    hexprint(stdout, z, n);

    free(x);
    free(y);
    free(z);

    return 0;
}
