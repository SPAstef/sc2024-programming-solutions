#pragma once

#include <cinttypes>

namespace crypto::tc05
{
    static constexpr int ROUNDS = 8;

    void next_key(uint16_t keys[4], uint32_t i);

    uint16_t sigma(uint16_t m);
    uint16_t sigma_inv(uint16_t m);
    uint16_t sbox(uint16_t m);
    uint16_t sbox_inv(uint16_t m);
    void sched(uint16_t keys[4], uint64_t k, int rounds = ROUNDS);
    uint16_t feistel(uint16_t m);
    void next_key(uint16_t keys[4], uint32_t i);

    uint32_t enc(uint32_t m, uint64_t k, int rounds = ROUNDS);
    uint32_t dec(uint32_t m, uint64_t k, int rounds = ROUNDS);
    uint32_t dec_endkey(uint32_t m, uint64_t ek, int rounds = ROUNDS);
} // namespace crypto::tc05
