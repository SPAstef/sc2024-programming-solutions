#include "aes.h"
#include "fq/fq.h"
#include <inttypes.h>

int main()
{
    for (uint32_t i = 0; i < 256; ++i)
    {
        fq_t x = fq_from_int(i, 2);
        x = aes128_sbox(x);

        uint64_t y = fq_to_int(x, 2);

        printf("%02x -> %02" PRIx64 " \n", i, y);

        fq_del(x);
    }

    return 0;
}
