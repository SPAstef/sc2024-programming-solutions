#include "rand.h"

#include <random>

uint32_t prand32(uint32_t lo, uint32_t hi)
{
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<uint32_t> dis{lo, hi};

    return dis(gen);
}

uint64_t prand64(uint64_t lo, uint64_t hi)
{
    static std::mt19937_64 gen(std::random_device{}());
    static std::uniform_int_distribution<uint64_t> dis{lo, hi};

    return dis(gen);
}

void randbytes(uint8_t *buf, size_t len)
{
    using uint = std::random_device::result_type;

    static constexpr size_t N = sizeof(uint);

    static std::random_device rng;

    size_t q = len / N;
    size_t r = len % N;
    uint *buf_large = reinterpret_cast<uint *>(buf);

    for (size_t i = 0; i < q; ++i)
        buf_large[i] = rng();

    if (r)
    {
        uint last = rng();

        for (size_t i = 0; i < r; ++i)
            buf[i + q * N] = (last >> (8 * i)) & 0xff;
    }
}
