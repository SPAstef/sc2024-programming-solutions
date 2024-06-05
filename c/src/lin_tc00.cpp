#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <numeric>
#include <print>
#include <random>
#include <vector>

#include "intrinsics.h"

static constexpr size_t KNOWN_MSG_N = 16;
static constexpr size_t EXPERIMENTS_N = 100;


static constexpr std::array<uint8_t, 16> SBOX{9, 11, 12, 4, 10, 1, 2, 6, 13, 7, 3, 8, 15, 14, 0, 5};
static constexpr std::array<uint8_t, 16> IBOX{14, 5, 6, 10, 3, 15, 7, 9, 11, 0, 4, 1, 2, 8, 13, 12};

uint8_t enc(uint8_t msg, uint8_t key)
{

    return SBOX[(msg ^ (key & 0xF)) & 0xF] ^ (key >> 4);
}

uint8_t dec(uint8_t cip, uint8_t key)
{

    return (IBOX[(cip ^ (key >> 4)) & 0xF] ^ key) & 0xF;
}

uint8_t hw(uint8_t x)
{
    return _popcnt32(x);
}

bool hp(uint8_t x)
{
    return hw(x) & 1;
}

int8_t eq_score(uint16_t eq)
{
    return hp(eq) ? -1 : 1;
}

uint32_t attack(uint8_t real_key, uint8_t good_in, uint8_t good_out)
{
    std::vector<uint8_t> msg(KNOWN_MSG_N);
    std::vector<uint8_t> cip(KNOWN_MSG_N);

    std::println("Collecting plaintexts/ciphertexts...");

    for (size_t i = 0; i < KNOWN_MSG_N; ++i)
    {
        msg[i] = i;
        cip[i] = enc(msg[i], real_key);
    }

    std::println("Collected {} plaintexts/ciphertexts", KNOWN_MSG_N);

    std::println("Guessing second subkey...");

    std::array<int32_t, 16> score{};

    for (uint8_t k1 = 0; k1 < 16; ++k1)
    {
        for (size_t i = 0; i < msg.size(); ++i)
        {
            uint8_t d = IBOX[cip[i] ^ k1];

            score[k1] += eq_score((d & good_in) ^ (msg[i] & good_in));
        }
        score[k1] *= score[k1]; // absolute value or squaring
    }

    std::array<uint8_t, 16> ranked_k1;
    std::ranges::iota(ranked_k1, 0);
    std::ranges::sort(ranked_k1, [&](uint8_t x, uint8_t y) { return score[x] > score[y]; });

    std::print("Top 4 candidates: ");
    for (size_t i = 0; i < 4; ++i)
        std::print("{:x} ", ranked_k1[i]);
    std::println("");

    bool found = false;
    uint8_t key = 0;
    uint32_t attempts = 0;
    for (size_t i = 0; i < ranked_k1.size() && !found; ++i, ++attempts)
    {
        uint8_t k1 = ranked_k1[i];

        std::println("Trying {:x}...", k1);

        for (uint8_t k0 = 0; k0 < 16 && !found; ++k0)
        {
            key = (k1 << 4) | (k0);

            bool maybe_found = true;
            for (size_t i = 0; maybe_found && i < msg.size(); ++i)
                maybe_found &= enc(msg[i], key) == cip[i];

            found = maybe_found;
        }
    }

    std::println("Recovered key: {:02x}", key);
    std::println("Real key:      {:02x}", real_key);

    return attempts;
}


int main()
{
    // Test encryption decryption
    std::mt19937 prng{std::random_device{}()};
    uint8_t real_key = prng();

    for (uint8_t x = 0; x < 16; ++x)
        assert(dec(enc(x, real_key), real_key) == x);

    std::println("Building LAT...");
    std::array<uint8_t, 256> lat{};

    for (uint8_t mx = 0; mx < 16; ++mx)
        for (uint8_t my = 0; my < 16; ++my)
            for (uint8_t x = 0; x < 16; ++x)
                if (hp(x & mx) == hp(SBOX[x] & my))
                    ++lat[mx * 16 + my];

    std::println("LAT:");
    std::print("   ");
    for (size_t i = 0; i < 16; ++i)
        std::print("{:2x} ", i);
    std::println("");
    for (size_t i = 0; i < 16; ++i)
    {
        std::print("{:2x}", i);
        for (size_t j = 0; j < 16; ++j)
            std::print(" {:2}", lat[i * 16 + j]);
        std::println("");
    }
    std::println("");

    std::println("Deriving LBT...");
    std::array<float, 256> lbt{};

    for (size_t i = 0; i < lat.size(); ++i)
        lbt[i] = (static_cast<float>(lat[i]) - 8.0f) / 16.0f;

    std::println("LBT:");
    std::print("   ");
    for (size_t i = 0; i < 16; ++i)
        std::print("{:6x} ", i);
    std::println("");

    for (size_t i = 0; i < 16; ++i)
    {
        std::print("{:2x}", i);
        for (size_t j = 0; j < 16; ++j)
            std::print(" {:6}", lbt[i * 16 + j]);
        std::println("");
    }
    std::println("");

    auto [min_iter, max_iter] = std::minmax_element(lbt.begin() + 1, lbt.end());
    size_t good_idx = (std::abs(*max_iter) > std::abs(*min_iter) ? max_iter : min_iter) -
                      lbt.begin();
    uint8_t good_in = good_idx / 16;
    uint8_t good_out = good_idx % 16;

    std::println("Found good linear approximation: {:2x} = {:2x} (e = {})", good_in, good_out,
                 lbt[good_idx]);

    float avg_attempts = 0.0f;

    for (size_t i = 0; i < EXPERIMENTS_N; ++i)
    {
        avg_attempts += attack(prng(), good_in, good_out);
        std::println("");
    }

    avg_attempts /= EXPERIMENTS_N;

    std::println("Average attempts over {} runs: {} (with bruteforce, expected 8)", EXPERIMENTS_N,
                 avg_attempts);

    return 0;
}
