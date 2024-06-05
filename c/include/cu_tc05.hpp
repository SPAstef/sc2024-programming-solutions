#pragma once

#include "tc05.hpp"

#include <cinttypes>
#include <span>

namespace cu::crypto::tc05
{
    static constexpr uint32_t ROUNDS_N = ::crypto::tc05::ROUNDS;

    uint32_t test_enc(uint32_t msg, uint64_t key); 
    // input is last 4 subkeys in the following layout: sk[n-1] || sk[n-2] || sk[n-3] || sk[n-4]
    uint32_t test_dec(uint32_t cip, uint64_t last_key);

    uint64_t crack(std::span<uint32_t> msg, std::span<uint32_t> cip, uint16_t skn2, uint16_t skn1, uint64_t off = 0, size_t watch = 0);
} // namespace cu::crypto::tc05
