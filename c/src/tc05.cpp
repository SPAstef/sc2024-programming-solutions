#include "tc05.hpp"

namespace crypto::tc05
{
    uint16_t sigma(uint16_t word)
    {
        uint32_t new_word = 0;

        new_word |= (word & 0xC00C) >> 1;
        new_word |= (word & 0x0020) >> 2;
        new_word |= (word & 0x0010) >> 4;
        new_word |= (word & 0x0C00) >> 5;
        new_word |= (word & 0x2000) >> 6;
        new_word |= (word & 0x1000) >> 8;

        new_word |= (word & 0x00C0) << 3;
        new_word |= (word & 0x0100) << 4;
        new_word |= (word & 0x0200) << 6;
        new_word |= (word & 0x0001) << 8;
        new_word |= (word & 0x0002) << 10;

        return (uint16_t)new_word;
    }

    uint16_t sigma_inv(uint16_t word)
    {
        uint32_t new_word = 0;

        new_word |= (word & (0x0002 << 10)) >> 10;
        new_word |= (word & (0x0001 << 8)) >> 8;
        new_word |= (word & (0x0200 << 6)) >> 6;
        new_word |= (word & (0x0100 << 4)) >> 4;
        new_word |= (word & (0x00C0 << 3)) >> 3;

        new_word |= (word & (0x1000 >> 8)) << 8;
        new_word |= (word & (0x2000 >> 6)) << 6;
        new_word |= (word & (0x0C00 >> 5)) << 5;
        new_word |= (word & (0x0010 >> 4)) << 4;
        new_word |= (word & (0x0020 >> 2)) << 2;
        new_word |= (word & (0xC00C >> 1)) << 1;

        return (uint16_t)new_word;
    }

    void next_key(uint16_t keys[4], uint32_t i)
    {
        keys[i & 3] = (uint16_t)(keys[i & 3] ^ keys[(i - 1) & 3] ^ sigma(keys[(i - 2) & 3]) ^ 0xC);
    }

    uint16_t sbox(uint16_t m)
    {
        static constexpr uint8_t sbox[16] = {0xE, 0xB, 0x4, 0x6, 0xA, 0xD, 0x7, 0x0,
                                             0x3, 0x8, 0xF, 0xC, 0x5, 0x9, 0x1, 0x2};
        uint16_t p = sbox[m & 0xF];

        p |= (uint16_t)((uint16_t)sbox[m >> 4 & 0xF] << 4);
        p |= (uint16_t)((uint16_t)sbox[m >> 8 & 0xF] << 8);
        p |= (uint16_t)((uint16_t)sbox[m >> 12] << 12);

        return (uint16_t)p;
    }

    uint16_t sbox_inv(uint16_t m)
    {
        static constexpr uint8_t sbox[16] = {0x7, 0xe, 0xf, 0x8, 0x2, 0xc, 0x3, 0x6,
                                             0x9, 0xd, 0x4, 0x1, 0xb, 0x5, 0x0, 0xa};
        uint16_t p = sbox[m & 0xF];

        p |= (uint16_t)((uint16_t)sbox[m >> 4 & 0xF] << 4);
        p |= (uint16_t)((uint16_t)sbox[m >> 8 & 0xF] << 8);
        p |= (uint16_t)((uint16_t)sbox[m >> 12] << 12);

        return (uint16_t)p;
    }

    uint16_t F(uint16_t m)
    {
        return sigma(sbox(m));
    }

    uint32_t round(uint32_t m, uint16_t k)
    {
        uint16_t l = (uint16_t)(m >> 16);
        uint16_t r = (uint16_t)m;

        return ((uint32_t)(F(l) ^ r ^ k) << 16) | l;
    }

    uint32_t enc(uint32_t m, uint64_t k, int rounds)
    {
        uint16_t keys[4] = {(uint16_t)(k >> 48), (uint16_t)(k >> 32), //
                            (uint16_t)(k >> 16), (uint16_t)(k >> 0)};

        for (int i = 0; i < rounds; ++i)
        {
            m = round(m, keys[i & 3]);
            next_key(keys, i);
        }

        return m;
    }

    void sched(uint16_t keys[4], uint64_t k, int rounds)
    {
        keys[0] = (uint16_t)(k >> 48);
        keys[1] = (uint16_t)(k >> 32);
        keys[2] = (uint16_t)(k >> 16);
        keys[3] = (uint16_t)(k >> 0);

        for (int i = 0; i < rounds; ++i)
            next_key(keys, i);
    }

    uint32_t dec(uint32_t m, uint64_t k, int rounds)
    {
        uint16_t keys[4];

        sched(keys, k, rounds);

        m = m >> 16 | m << 16;
        for (int i = rounds - 1; i >= 0; --i)
        {
            next_key(keys, i);
            m = round(m, keys[i & 3]);
        }

        return m >> 16 | m << 16;
    }

    uint32_t dec_endkey(uint32_t m, uint64_t ek, int rounds)
    {
        uint16_t keys[4];

        keys[(rounds - 1) & 3] = ek >> 48;
        keys[(rounds - 2) & 3] = ek >> 32;
        keys[(rounds - 3) & 3] = ek >> 16;
        keys[(rounds - 4) & 3] = ek >> 0;

        m = m >> 16 | m << 16;
        for (int i = rounds - 1; i >= 0; --i)
        {
            m = round(m, keys[i & 3]);
            next_key(keys, i);
        }

        return m >> 16 | m << 16;
    }
} // namespace crypto::tc05
