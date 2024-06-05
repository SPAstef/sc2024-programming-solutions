#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <numeric>
#include <print>
#include <random>
#include <vector>

static constexpr size_t CHOSEN_MSG_N = 16;
static constexpr size_t GOOD_MSG_N = 6;
static constexpr size_t EXPERIMENTS = 256;


static constexpr std::array<uint8_t, 16> SBOX{3, 14, 1, 10, 4, 9, 5, 6, 8, 11, 15, 2, 13, 12, 0, 7};
static constexpr std::array<uint8_t, 16> IBOX{14, 2, 11, 0, 4, 6, 7, 15, 8, 5, 3, 9, 13, 12, 1, 10};

uint8_t enc(uint8_t msg, uint8_t key)
{

    return SBOX[(msg ^ (key & 0xF)) & 0xF] ^ (key >> 4);
}

uint8_t dec(uint8_t cip, uint8_t key)
{

    return (IBOX[(cip ^ (key >> 4)) & 0xF] ^ key) & 0xF;
}


int main()
{
    // Test encryption decryption
    std::mt19937 prng{std::random_device{}()};

    for (uint8_t x = 0, k = prng(); x < 16; ++x)
        assert(dec(enc(x, k), k) == x);


    std::array<uint8_t, 256> ddt{};

    std::println("Building DDT...");

    for (uint8_t dx = 0; dx < 16; ++dx)
        for (uint8_t x = 0; x < 16; ++x)
        {
            uint8_t dy = SBOX[x] ^ SBOX[x ^ dx];
            ++ddt[dx * 16 + dy];
        }

    std::println("DDT:");
    std::print("   ");
    for (size_t i = 0; i < 16; ++i)
        std::print("{:2x} ", i);
    std::println("");
    for (size_t i = 0; i < 16; ++i)
    {
        std::print("{:2x}", i);
        for (size_t j = 0; j < 16; ++j)
            std::print(" {:2}", ddt[i * 16 + j]);
        std::println("");
    }
    std::println("");


    uint8_t good_idx = std::max_element(ddt.begin() + 1, ddt.end()) - ddt.begin();
    uint8_t good_in = good_idx / 16;
    uint8_t good_out = good_idx % 16;

    std::println("Found good differential: ({:x} -({}/16)-> {:x})", good_in, ddt[good_idx],
                 good_out);

    std::println("Collecting plaintext/ciphertext pairs...");

    size_t rank_avg = 0;
    for (size_t i = 0; i < EXPERIMENTS; ++i)
    {
        uint8_t real_key = static_cast<uint8_t>(i);

        // Choose plaintexts and query ciphertexts
        std::vector<std::pair<uint8_t, uint8_t>> good_msg;
        std::vector<std::pair<uint8_t, uint8_t>> good_cip;
        std::vector<std::pair<uint8_t, uint8_t>> bad_msg;
        std::vector<std::pair<uint8_t, uint8_t>> bad_cip;

        for (size_t i = 0; i < CHOSEN_MSG_N; ++i)
        {
            std::pair<uint8_t, uint8_t> m{i, i ^ good_in};
            std::pair<uint8_t, uint8_t> c{enc(m.first, real_key), enc(m.second, real_key)};

            // Filter good ciphertext pairs
            if ((c.first ^ c.second) == good_out && good_msg.size() < GOOD_MSG_N)
            {
                good_msg.emplace_back(m);
                good_cip.emplace_back(c);
            }
            else // we still keep the bad ones for validating key guesses
            {
                bad_msg.emplace_back(m);
                bad_cip.emplace_back(c);
            }
        }

        std::println("Found {}/{} good pairs", good_msg.size(), CHOSEN_MSG_N);

        std::println("Guessing second subkey...");

        std::array<uint32_t, 16> score{};

        for (uint8_t k1 = 0; k1 < 16; ++k1)
        {
            for (size_t i = 0; i < good_msg.size(); ++i)
            {
                uint8_t d1 = IBOX[good_cip[i].first ^ k1];
                uint8_t d2 = IBOX[good_cip[i].second ^ k1];

                if ((d1 ^ d2) == good_in)
                    ++score[k1];
            }
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

        for (size_t i = 0; i < ranked_k1.size() && !found; ++i)
        {
            key = ranked_k1[i] << 4;

            std::println("Trying {:x}", ranked_k1[i]);
            ++rank_avg;

            for (uint8_t k0 = 0; k0 < 16 && !found; ++k0)
            {
                key = (key & 0xf0) | k0;

                bool maybe_found = true;
                for (size_t i = 0; maybe_found && i < good_msg.size(); ++i)
                {
                    maybe_found &= enc(good_msg[i].first, key) == good_cip[i].first;
                    maybe_found &= enc(good_msg[i].second, key) == good_cip[i].second;
                }
                for (size_t i = 0; maybe_found && i < bad_msg.size(); ++i)
                {
                    maybe_found &= enc(bad_msg[i].first, key) == bad_cip[i].first;
                    maybe_found &= enc(bad_msg[i].second, key) == bad_cip[i].second;
                }

                found = maybe_found;
            }
        }

        std::println("Recovered key: {:02x}", key);
        std::println("Real key:      {:02x}", real_key);
    }

    std::println("================");
    std::println("Average rank of correct key: {}", static_cast<double>(rank_avg) / EXPERIMENTS);

    return 0;
}
