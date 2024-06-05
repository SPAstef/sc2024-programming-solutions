#include "intrinsics.h"
#include "tc05.hpp"

#ifdef USE_CUDA
    #include "cu_tc05.hpp"
#endif

#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <fstream>
#include <numeric>
#include <omp.h>
#include <print>
#include <random>
#include <span>
#include <string>

using namespace std::string_literals;

namespace tc05 = crypto::tc05;

#ifdef USE_CUDA
namespace cu_tc05 = cu::crypto::tc05;
#endif

static constexpr size_t ROUNDS_N = tc05::ROUNDS;
static const std::string DB_FNAME{"/mnt/OS/Users/stefa/Documents/dottorato/repo/"
                                  "symmetric-crypto-2024/programming_solutions/c/tc05_data_" +
                                  std::to_string(ROUNDS_N) + "r.txt"};

//static constexpr size_t MSG_N = 1ULL << 32;
static constexpr size_t SUBKEYS_N = 1ULL << 16;
static constexpr size_t KNOWN_MSG_N = 1ULL << 16;
static constexpr size_t EXPERIMENTS_N = 1;
static constexpr size_t SKN2_TRIALS = 16;

using MsgP = std::pair<uint32_t, uint32_t>;
using LAT = std::array<uint8_t, 256>;
using BTab = std::array<double, 256>;

struct InOutP
{
    uint16_t in;
    uint16_t out;
    double e;
};

struct Mask3
{
    uint16_t l;
    uint16_t r;
    uint16_t o;
};


using LinPath = std::array<InOutP, ROUNDS_N>;

uint8_t hw(uint32_t x)
{
    return _popcnt32(x);
}

bool hp(uint32_t x)
{
    return hw(x) & 1;
}

int eq_score(uint16_t eq)
{
    return 2 * static_cast<int>(hp(eq)) - 1;
}

LAT build_lat()
{
    LAT lat{};

    for (uint8_t x = 0; x < 16; ++x)
    {
        uint8_t y = tc05::sbox(x);

        for (uint8_t msk_in = 0; msk_in < 16; ++msk_in)
            for (uint8_t msk_out = 0; msk_out < 16; ++msk_out)
                lat[msk_in * 16 + msk_out] += hp(x & msk_in) == hp(y & msk_out);
    }

    return lat;
}

BTab build_btab(const LAT &lat)
{
    BTab btab;

    for (size_t i = 0; i < btab.size(); ++i)
        btab[i] = ((double)lat[i] - 8) / 16;

    return btab;
}

double combine_bias(double e1, double e2)
{
    // Use the Piling-Up Lemma to combine biases
    return 2 * e1 * e2;
}

double mask_score(uint8_t msk, double bias)
{
    return std::abs(bias);
}

uint8_t best_btab_column(const double *row)
{
    uint8_t best_i = 0;
    double best_score = 0.0;

    for (uint8_t i = 0; i < 16; ++i)
        if (double score = mask_score(i, row[i]); score > best_score)
        {
            best_i = i;
            best_score = score;
        }

    return best_i;
}

LinPath find_good_path(const BTab &btab)
{
    static constexpr size_t PATHS_N = 1;
    static constexpr uint16_t FIRST_MSK_IN = 0x000d;
    static constexpr uint16_t FIRST_MSK_OUT = 0x0002;

    LinPath best_path{};

    for (size_t i = 0; i < PATHS_N; ++i)
    {
        LinPath path{};
        // zero would be the best of course, but causes no activation
        path[0].in = FIRST_MSK_IN;
        path[0].out = FIRST_MSK_OUT;
        path[0].e = 0.5;

        for (uint8_t nib = 0; nib < 4; ++nib)
        {
            uint8_t in_msk = path[0].in >> 4 * nib & 0xF;
            uint8_t out_msk = path[0].out >> 4 * nib & 0xF;

            path[0].e = combine_bias(path[0].e, btab[in_msk * 16 + out_msk]);
        }

        for (size_t r = 1; r < ROUNDS_N; ++r)
        {
            path[r].in = tc05::sigma(path[r - 1].out);
            path[r].e = path[r - 1].e;

            for (uint8_t nib = 0; nib < 4; ++nib)
            {
                uint8_t in_msk = path[r].in >> 4 * nib & 0xF;
                uint16_t out_msk = best_btab_column(btab.data() + in_msk * 16);

                path[r].out |= out_msk << 4 * nib;
                path[r].e = combine_bias(path[r].e, btab[in_msk * 16 + out_msk]);
            }
        }
        if (std::abs(path.back().e) > std::abs(best_path.back().e))
            best_path = path;
    }

    return best_path;
}

Mask3 find_good_mask_triple()
{
    static constexpr size_t SAMPLE_N = 1000;
    Mask3 msk{};
    int best_score = 0;
    std::mt19937 rng{std::random_device{}()};
    std::vector<int> lat(SUBKEYS_N * SUBKEYS_N);
    std::array<uint16_t, SAMPLE_N> msk_l;
    std::array<uint16_t, SAMPLE_N> msk_r;
    std::array<uint16_t, SAMPLE_N> msk_o;

    std::ranges::generate(msk_l, rng);
    std::ranges::generate(msk_r, rng);
    std::ranges::generate(msk_o, rng);

#pragma omp parallel for
    for (uint32_t i = 0; i < SAMPLE_N; ++i)
    {
        if (omp_get_thread_num() == 0)
            std::print("Progress: {:.2f}%\r", (float)i * 100 / SUBKEYS_N);
        for (uint32_t x = 0; x < SUBKEYS_N; ++x)
        {
            uint16_t xl = x >> 16;
            uint32_t y = tc05::enc(x, 0, ROUNDS_N - 3);
            uint16_t yl = y >> 16;

            for (uint32_t j = 0; j < SAMPLE_N; ++j)
                lat[msk_l[i] * SUBKEYS_N + msk_o[j]] += hp(xl & msk_l[i]) == hp(yl & msk_o[j]);
        }
    }
    std::println("");

    for (auto &&x : lat)
        if (x)
        {
            x -= SUBKEYS_N;
            x *= x;
        }

    for (size_t i = 0; i < SUBKEYS_N; ++i)
    {
        lat[i] = 0;
        lat[i * SUBKEYS_N] = 0;
    }

    for (size_t i = 0; i < lat.size(); ++i)
        if (lat[i] > best_score)
        {
            msk.l = i / SUBKEYS_N;
            msk.o = i % SUBKEYS_N;
            best_score = lat[i];
        }

    return msk;
}


template<std::ranges::range R>
void print_table(const R &tab, size_t rows, size_t cols)
{
    std::print("\t");
    for (size_t i = 0; i < 16; ++i)
        std::print("{:x}\t", i);
    std::println("");
    for (size_t i = 0; i < 16; ++i)
        std::print("--------");
    std::println("");

    for (size_t i = 0; i < rows; ++i)
    {
        std::print("{:x} |\t", i);
        for (size_t j = 0; j < cols; ++j)
            std::print("{}\t", tab[i * cols + j]);
        std::println("");
    }
}

void print_path(const LinPath &path)
{
    for (size_t i = 0; i < path.size() - 1; ++i)
        std::println("({:04x}, {:04x}, {:.2e}) ->", path[i].in, path[i].out, path[i].e);
    std::println("({:04x}, {:04x}, {:.2e})", path.back().in, path.back().out, path.back().e);
}


uint64_t crack(std::span<uint32_t> msg, std::span<uint32_t> cip, uint16_t skn1, uint16_t skn2,
               uint64_t off = 0, size_t watch = 0)
{
    using namespace std::chrono;
    using clk = high_resolution_clock;
    static constexpr uint64_t RECOVER_SPACE = 1ULL << 32;
    uint64_t base_key = (uint64_t)skn1 << 48 | (uint64_t)skn2 << 32;
    uint64_t key = 0;

    auto start = clk::now();
    bool found = false;
#pragma omp parallel shared(key, found)
    {
        size_t th_id = omp_get_thread_num();
        size_t th_n = omp_get_num_threads();

        off += th_id;

        for (size_t i = 0; !found && off < RECOVER_SPACE; ++i, off += th_n)
        {
            uint64_t my_key = base_key | off;
            if (tc05::dec_endkey(cip[0], my_key, ROUNDS_N) == msg[0] &&
                tc05::dec_endkey(cip[1], my_key, ROUNDS_N) == msg[1])
            {
                key = my_key;
                found = true;
            }
            if (th_id == 0 && watch && !(i % watch))
            {
                auto elap = duration_cast<seconds>(clk::now() - start).count();

                std::print("{:<20}\t{} sec. ({:.2e} enc/s)\r", off, elap, (double)off / elap);
                std::fflush(stdout);
            }
        }
    }
    if (watch)
        std::println("");

    if (key)
    {
        uint16_t sk[4];

        sk[(ROUNDS_N - 1) & 3] = key >> 48;
        sk[(ROUNDS_N - 2) & 3] = key >> 32;
        sk[(ROUNDS_N - 3) & 3] = key >> 16;
        sk[(ROUNDS_N - 4) & 3] = key >> 0;

        for (uint32_t i = ROUNDS_N - 1; i >= 4; --i)
            tc05::next_key(sk, i);

        key = sk[3];
        key |= static_cast<uint64_t>(sk[2]) << 16;
        key |= static_cast<uint64_t>(sk[1]) << 32;
        key |= static_cast<uint64_t>(sk[0]) << 48;
    }

    return key;
}

uint64_t linear_attack(std::span<MsgP> known_msg, uint64_t real_key = 0)
{
    std::array<uint16_t, 4> real_subkeys{(uint16_t)(real_key >> 48), (uint16_t)(real_key >> 32), //
                                         (uint16_t)(real_key >> 16), (uint16_t)(real_key >> 0)};
    uint16_t real_skn1 = 0;
    uint16_t real_skn2 [[maybe_unused]] = 0;
    size_t real_skn1_rank = 0;
    std::array<int32_t, SUBKEYS_N>
        sk_score{}; // NOTE! 32-bits might not be enough if >= 2^16 messages are used

    if (real_key)
    {
        //std::println("Real subkeys: ");
        for (size_t i = 0; i < ROUNDS_N - 2; ++i)
        {
            //std::println("{}: {:04x}", i, real_subkeys[i & 3]);
            tc05::next_key(real_subkeys.data(), i);
        }
        real_skn1 = real_subkeys[(ROUNDS_N - 1) & 3];
        real_skn2 = real_subkeys[(ROUNDS_N - 2) & 3];
        //std::println("{}: {:04x}", ROUNDS_N - 2, real_skn2);
        //std::println("{}: {:04x}", ROUNDS_N - 1, real_skn1);
    }

#pragma omp parallel for
    for (uint32_t sk = 0; sk < SUBKEYS_N; ++sk)
    {
        std::array<int, 24> subscore{};

        for (auto [pln, cip] : known_msg)
        {
            uint16_t l0 = pln >> 16;
            uint16_t r0 = pln;
            uint16_t ln = cip >> 16;
            uint16_t rn = cip;
            uint16_t grn1 = ln ^ tc05::sigma(tc05::sbox(rn)) ^ sk;
            uint16_t gln3 = (tc05::sigma(tc05::sbox(grn1)) ^ rn);

            if constexpr (ROUNDS_N == 5)
            {
                // use multiple approximations to improve accuracy
                subscore[0] += eq_score((gln3 & 0x0009) ^ (l0 & 0x0a09) ^ (r0 & 0x0040));
                subscore[1] += eq_score((gln3 & 0x0090) ^ (l0 & 0xa090) ^ (r0 & 0x4000));
                subscore[2] += eq_score((gln3 & 0x0900) ^ (l0 & 0x090a) ^ (r0 & 0x0004));
                subscore[3] += eq_score((gln3 & 0x9000) ^ (l0 & 0x90a0) ^ (r0 & 0x0400));

                subscore[4] += eq_score((gln3 & 0x0006) ^ (l0 & 0x00d0) ^ (r0 & 0x000a));
                subscore[5] += eq_score((gln3 & 0x0060) ^ (l0 & 0x000d) ^ (r0 & 0x0a00));
                subscore[6] += eq_score((gln3 & 0x0600) ^ (l0 & 0xd000) ^ (r0 & 0x00a0));
                subscore[7] += eq_score((gln3 & 0x6000) ^ (l0 & 0x0d00) ^ (r0 & 0xa000));
            }
            else if constexpr (ROUNDS_N == 6)
            {
                subscore[0] += eq_score((gln3 & 0x0001) ^ (l0 & 0xa0a0) ^ (r0 & 0x4001));
                subscore[1] += eq_score((gln3 & 0x0010) ^ (l0 & 0xa0a0) ^ (r0 & 0x0410));
                subscore[2] += eq_score((gln3 & 0x0100) ^ (l0 & 0x0a0a) ^ (r0 & 0x0140));
                subscore[3] += eq_score((gln3 & 0x1000) ^ (l0 & 0x0a0a) ^ (r0 & 0x1004));

                subscore[4] += eq_score((gln3 & 0x0001) ^ (l0 & 0x20a0) ^ (r0 & 0x4001));
                subscore[5] += eq_score((gln3 & 0x0010) ^ (l0 & 0xa020) ^ (r0 & 0x0410));
                subscore[6] += eq_score((gln3 & 0x0100) ^ (l0 & 0x020a) ^ (r0 & 0x0140));
                subscore[7] += eq_score((gln3 & 0x1000) ^ (l0 & 0x0a02) ^ (r0 & 0x1004));

                subscore[8] += eq_score((gln3 & 0x0004) ^ (l0 & 0x0002) ^ (r0 & 0x0000));
                subscore[9] += eq_score((gln3 & 0x0040) ^ (l0 & 0x0200) ^ (r0 & 0x0000));
                subscore[10] += eq_score((gln3 & 0x0400) ^ (l0 & 0x0020) ^ (r0 & 0x0000));
                subscore[11] += eq_score((gln3 & 0x4000) ^ (l0 & 0x2000) ^ (r0 & 0x0000));
            }
            else if constexpr (ROUNDS_N == 7)
            {
                subscore[0] += eq_score((gln3 & 0x0009) ^ (l0 & 0x0809) ^ (r0 & 0x0060));
                subscore[1] += eq_score((gln3 & 0x0090) ^ (l0 & 0x8090) ^ (r0 & 0x6000));
                subscore[2] += eq_score((gln3 & 0x0900) ^ (l0 & 0x0908) ^ (r0 & 0x0006));
                subscore[3] += eq_score((gln3 & 0x9000) ^ (l0 & 0x9080) ^ (r0 & 0x0600));

                subscore[4] += eq_score((gln3 & 0x0002) ^ (l0 & 0x0002) ^ (r0 & 0x0004));
                subscore[5] += eq_score((gln3 & 0x0020) ^ (l0 & 0x0200) ^ (r0 & 0x0400));
                subscore[6] += eq_score((gln3 & 0x0200) ^ (l0 & 0x0020) ^ (r0 & 0x0040));
                subscore[7] += eq_score((gln3 & 0x2000) ^ (l0 & 0x2000) ^ (r0 & 0x4000));

                subscore[8] += eq_score((gln3 & 0x0004) ^ (l0 & 0x00d4) ^ (r0 & 0x000a));
                subscore[9] += eq_score((gln3 & 0x0040) ^ (l0 & 0x004d) ^ (r0 & 0x0a00));
                subscore[10] += eq_score((gln3 & 0x0400) ^ (l0 & 0xd400) ^ (r0 & 0x00a0));
                subscore[11] += eq_score((gln3 & 0x4000) ^ (l0 & 0x4d00) ^ (r0 & 0xa000));

                subscore[12] += eq_score((gln3 & 0x0002) ^ (l0 & 0x0002) ^ (r0 & 0x0006));
                subscore[13] += eq_score((gln3 & 0x0020) ^ (l0 & 0x0020) ^ (r0 & 0x0060));
                subscore[14] += eq_score((gln3 & 0x0200) ^ (l0 & 0x0200) ^ (r0 & 0x0600));
                subscore[15] += eq_score((gln3 & 0x2000) ^ (l0 & 0x2000) ^ (r0 & 0x6000));
            }
            else if constexpr (ROUNDS_N == 8)
            {
                subscore[0] += eq_score((gln3 & 0x0001) ^ (l0 & 0x2030) ^ (r0 & 0x2001));
                subscore[1] += eq_score((gln3 & 0x0010) ^ (l0 & 0x3020) ^ (r0 & 0x0210));
                subscore[2] += eq_score((gln3 & 0x0100) ^ (l0 & 0x0203) ^ (r0 & 0x0120));
                subscore[3] += eq_score((gln3 & 0x1000) ^ (l0 & 0x0302) ^ (r0 & 0x1002));

                subscore[4] += eq_score((gln3 & 0x0001) ^ (l0 & 0xa030) ^ (r0 & 0x2001));
                subscore[5] += eq_score((gln3 & 0x0010) ^ (l0 & 0x30a0) ^ (r0 & 0x0210));
                subscore[6] += eq_score((gln3 & 0x0100) ^ (l0 & 0x0a03) ^ (r0 & 0x0120));
                subscore[7] += eq_score((gln3 & 0x1000) ^ (l0 & 0x030a) ^ (r0 & 0x1002));

                subscore[8] += eq_score((gln3 & 0x0009) ^ (l0 & 0x002d) ^ (r0 & 0x0809));
                subscore[9] += eq_score((gln3 & 0x0090) ^ (l0 & 0x2d00) ^ (r0 & 0x8090));
                subscore[10] += eq_score((gln3 & 0x0900) ^ (l0 & 0x00d2) ^ (r0 & 0x0908));
                subscore[11] += eq_score((gln3 & 0x9000) ^ (l0 & 0xd200) ^ (r0 & 0x9080));

                subscore[12] += eq_score((gln3 & 0x0004) ^ (l0 & 0x0000) ^ (r0 & 0x0004));
                subscore[13] += eq_score((gln3 & 0x0040) ^ (l0 & 0x0000) ^ (r0 & 0x0040));
                subscore[14] += eq_score((gln3 & 0x0400) ^ (l0 & 0x0000) ^ (r0 & 0x0400));
                subscore[15] += eq_score((gln3 & 0x4000) ^ (l0 & 0x0000) ^ (r0 & 0x4000));

                subscore[16] += eq_score((gln3 & 0x0002) ^ (l0 & 0x0002) ^ (r0 & 0x0002));
                subscore[17] += eq_score((gln3 & 0x0020) ^ (l0 & 0x0200) ^ (r0 & 0x0020));
                subscore[18] += eq_score((gln3 & 0x0200) ^ (l0 & 0x0020) ^ (r0 & 0x0200));
                subscore[19] += eq_score((gln3 & 0x2000) ^ (l0 & 0x2000) ^ (r0 & 0x2000));
            }
            else if constexpr (ROUNDS_N == 9)
            {
                subscore[0] += eq_score((gln3 & 0x0009) ^ (l0 & 0x0809) ^ (r0 & 0x0020));
                subscore[1] += eq_score((gln3 & 0x0090) ^ (l0 & 0x8090) ^ (r0 & 0x2000));
                subscore[2] += eq_score((gln3 & 0x0900) ^ (l0 & 0x0908) ^ (r0 & 0x0002));
                subscore[3] += eq_score((gln3 & 0x9000) ^ (l0 & 0x9080) ^ (r0 & 0x0200));

                subscore[4] += eq_score((gln3 & 0x0004) ^ (l0 & 0x00d6) ^ (r0 & 0x0008));
                subscore[5] += eq_score((gln3 & 0x0040) ^ (l0 & 0x006d) ^ (r0 & 0x0800));
                subscore[6] += eq_score((gln3 & 0x0400) ^ (l0 & 0xd600) ^ (r0 & 0x0080));
                subscore[7] += eq_score((gln3 & 0x4000) ^ (l0 & 0x6d00) ^ (r0 & 0x8000));

                subscore[8] += eq_score((gln3 & 0x0001) ^ (l0 & 0x0d01) ^ (r0 & 0xa090));
                subscore[9] += eq_score((gln3 & 0x0010) ^ (l0 & 0xd010) ^ (r0 & 0x90a0));
                subscore[10] += eq_score((gln3 & 0x0100) ^ (l0 & 0x010d) ^ (r0 & 0x0a09));
                subscore[11] += eq_score((gln3 & 0x1000) ^ (l0 & 0x10d0) ^ (r0 & 0x090a));

                subscore[12] += eq_score((gln3 & 0x0104) ^ (l0 & 0x21c4) ^ (r0 & 0x220b));
                subscore[13] += eq_score((gln3 & 0x0401) ^ (l0 & 0xc421) ^ (r0 & 0x22b0));
                subscore[14] += eq_score((gln3 & 0x1040) ^ (l0 & 0x124c) ^ (r0 & 0x0b22));
                subscore[15] += eq_score((gln3 & 0x4010) ^ (l0 & 0x4c12) ^ (r0 & 0xb022));

                subscore[16] += eq_score((gln3 & 0x0002) ^ (l0 & 0x0002) ^ (r0 & 0x0000));
                subscore[17] += eq_score((gln3 & 0x0020) ^ (l0 & 0x0020) ^ (r0 & 0x0000));
                subscore[18] += eq_score((gln3 & 0x0200) ^ (l0 & 0x0200) ^ (r0 & 0x0000));
                subscore[19] += eq_score((gln3 & 0x2000) ^ (l0 & 0x2000) ^ (r0 & 0x0000));

                subscore[20] += eq_score((gln3 & 0x0002) ^ (l0 & 0x0002) ^ (r0 & 0x0002));
                subscore[21] += eq_score((gln3 & 0x0020) ^ (l0 & 0x0020) ^ (r0 & 0x0200));
                subscore[22] += eq_score((gln3 & 0x0200) ^ (l0 & 0x0200) ^ (r0 & 0x0020));
                subscore[23] += eq_score((gln3 & 0x2000) ^ (l0 & 0x2000) ^ (r0 & 0x2000));
            }
        }

        sk_score[sk] = 0;
        for (auto &&s : subscore)
            sk_score[sk] += (int64_t)s * s;
    }

    std::array<uint16_t, SUBKEYS_N> skn1_ranked;

    std::ranges::iota(skn1_ranked, 0);
    std::ranges::sort(skn1_ranked,
                      [&](uint16_t x, uint16_t y) { return sk_score[x] > sk_score[y]; });

    if (real_key)
    {
        for (size_t i = 0; i < skn1_ranked.size(); ++i)
            if (skn1_ranked[i] == real_skn1)
                real_skn1_rank = i;

        std::println("Real sk[N-1] rank: {}", real_skn1_rank, SUBKEYS_N);
    }

    std::vector<uint32_t> known_pln(known_msg.size());
    std::vector<uint32_t> known_cip(known_msg.size());

    for (size_t i = 0; i < known_msg.size(); ++i)
    {
        known_pln[i] = known_msg[i].first;
        known_cip[i] = known_msg[i].second;
    }

    uint64_t key = 0;

    for (size_t i = 0; !key && i < skn1_ranked.size(); ++i)
    {
        std::array<uint16_t, SUBKEYS_N> skn2_ranked;

#pragma omp parallel for
        for (uint32_t sk = 0; sk < SUBKEYS_N; ++sk)
        {
            std::array<int, 20> subscore{};

            for (auto [pln, cip] : known_msg)
            {
                uint16_t l0 = pln >> 16;
                uint16_t r0 = pln;
                uint16_t ln = cip >> 16;
                uint16_t rn = cip;
                uint16_t grn1 = ln ^ tc05::sigma(tc05::sbox(rn)) ^ skn1_ranked[i];
                uint16_t ln1 = rn;
                uint16_t grn2 = ln1 ^ tc05::sigma(tc05::sbox(grn1)) ^ sk;
                uint16_t gln4 = (tc05::sigma(tc05::sbox(grn2)) ^ grn1);

                if constexpr (ROUNDS_N == 6)
                {
                    // use multiple approximations to improve accuracy
                    subscore[0] += eq_score((gln4 & 0x0009) ^ (l0 & 0x0a09) ^ (r0 & 0x0040));
                    subscore[1] += eq_score((gln4 & 0x0090) ^ (l0 & 0xa090) ^ (r0 & 0x4000));
                    subscore[2] += eq_score((gln4 & 0x0900) ^ (l0 & 0x090a) ^ (r0 & 0x0004));
                    subscore[3] += eq_score((gln4 & 0x9000) ^ (l0 & 0x90a0) ^ (r0 & 0x0400));

                    subscore[4] += eq_score((gln4 & 0x0006) ^ (l0 & 0x00d0) ^ (r0 & 0x000a));
                    subscore[5] += eq_score((gln4 & 0x0060) ^ (l0 & 0x000d) ^ (r0 & 0x0a00));
                    subscore[6] += eq_score((gln4 & 0x0600) ^ (l0 & 0xd000) ^ (r0 & 0x00a0));
                    subscore[7] += eq_score((gln4 & 0x6000) ^ (l0 & 0x0d00) ^ (r0 & 0xa000));
                }
                else if constexpr (ROUNDS_N == 7)
                {
                    subscore[0] += eq_score((gln4 & 0x0001) ^ (l0 & 0xa0a0) ^ (r0 & 0x4001));
                    subscore[1] += eq_score((gln4 & 0x0010) ^ (l0 & 0xa0a0) ^ (r0 & 0x0410));
                    subscore[2] += eq_score((gln4 & 0x0100) ^ (l0 & 0x0a0a) ^ (r0 & 0x0140));
                    subscore[3] += eq_score((gln4 & 0x1000) ^ (l0 & 0x0a0a) ^ (r0 & 0x1004));

                    subscore[4] += eq_score((gln4 & 0x0001) ^ (l0 & 0x20a0) ^ (r0 & 0x4001));
                    subscore[5] += eq_score((gln4 & 0x0010) ^ (l0 & 0xa020) ^ (r0 & 0x0410));
                    subscore[6] += eq_score((gln4 & 0x0100) ^ (l0 & 0x020a) ^ (r0 & 0x0140));
                    subscore[7] += eq_score((gln4 & 0x1000) ^ (l0 & 0x0a02) ^ (r0 & 0x1004));

                    subscore[8] += eq_score((gln4 & 0x0004) ^ (l0 & 0x0002) ^ (r0 & 0x0000));
                    subscore[9] += eq_score((gln4 & 0x0040) ^ (l0 & 0x0200) ^ (r0 & 0x0000));
                    subscore[10] += eq_score((gln4 & 0x0400) ^ (l0 & 0x0020) ^ (r0 & 0x0000));
                    subscore[11] += eq_score((gln4 & 0x4000) ^ (l0 & 0x2000) ^ (r0 & 0x0000));
                }
                else if constexpr (ROUNDS_N == 8)
                {
                    subscore[0] += eq_score((gln4 & 0x0009) ^ (l0 & 0x0809) ^ (r0 & 0x0060));
                    subscore[1] += eq_score((gln4 & 0x0090) ^ (l0 & 0x8090) ^ (r0 & 0x6000));
                    subscore[2] += eq_score((gln4 & 0x0900) ^ (l0 & 0x0908) ^ (r0 & 0x0006));
                    subscore[3] += eq_score((gln4 & 0x9000) ^ (l0 & 0x9080) ^ (r0 & 0x0600));

                    subscore[4] += eq_score((gln4 & 0x0002) ^ (l0 & 0x0002) ^ (r0 & 0x0004));
                    subscore[5] += eq_score((gln4 & 0x0020) ^ (l0 & 0x0200) ^ (r0 & 0x0400));
                    subscore[6] += eq_score((gln4 & 0x0200) ^ (l0 & 0x0020) ^ (r0 & 0x0040));
                    subscore[7] += eq_score((gln4 & 0x2000) ^ (l0 & 0x2000) ^ (r0 & 0x4000));

                    subscore[8] += eq_score((gln4 & 0x0004) ^ (l0 & 0x00d4) ^ (r0 & 0x000a));
                    subscore[9] += eq_score((gln4 & 0x0040) ^ (l0 & 0x004d) ^ (r0 & 0x0a00));
                    subscore[10] += eq_score((gln4 & 0x0400) ^ (l0 & 0xd400) ^ (r0 & 0x00a0));
                    subscore[11] += eq_score((gln4 & 0x4000) ^ (l0 & 0x4d00) ^ (r0 & 0xa000));

                    subscore[12] += eq_score((gln4 & 0x0002) ^ (l0 & 0x0002) ^ (r0 & 0x0006));
                    subscore[13] += eq_score((gln4 & 0x0020) ^ (l0 & 0x0020) ^ (r0 & 0x0060));
                    subscore[14] += eq_score((gln4 & 0x0200) ^ (l0 & 0x0200) ^ (r0 & 0x0600));
                    subscore[15] += eq_score((gln4 & 0x2000) ^ (l0 & 0x2000) ^ (r0 & 0x6000));
                }
                else if constexpr (ROUNDS_N == 9)
                {
                    subscore[0] += eq_score((gln4 & 0x0001) ^ (l0 & 0x2030) ^ (r0 & 0x2001));
                    subscore[1] += eq_score((gln4 & 0x0010) ^ (l0 & 0x3020) ^ (r0 & 0x0210));
                    subscore[2] += eq_score((gln4 & 0x0100) ^ (l0 & 0x0203) ^ (r0 & 0x0120));
                    subscore[3] += eq_score((gln4 & 0x1000) ^ (l0 & 0x0302) ^ (r0 & 0x1002));

                    subscore[4] += eq_score((gln4 & 0x0001) ^ (l0 & 0xa030) ^ (r0 & 0x2001));
                    subscore[5] += eq_score((gln4 & 0x0010) ^ (l0 & 0x30a0) ^ (r0 & 0x0210));
                    subscore[6] += eq_score((gln4 & 0x0100) ^ (l0 & 0x0a03) ^ (r0 & 0x0120));
                    subscore[7] += eq_score((gln4 & 0x1000) ^ (l0 & 0x030a) ^ (r0 & 0x1002));

                    subscore[8] += eq_score((gln4 & 0x0009) ^ (l0 & 0x002d) ^ (r0 & 0x0809));
                    subscore[9] += eq_score((gln4 & 0x0090) ^ (l0 & 0x2d00) ^ (r0 & 0x8090));
                    subscore[10] += eq_score((gln4 & 0x0900) ^ (l0 & 0x00d2) ^ (r0 & 0x0908));
                    subscore[11] += eq_score((gln4 & 0x9000) ^ (l0 & 0xd200) ^ (r0 & 0x9080));

                    subscore[12] += eq_score((gln4 & 0x0004) ^ (l0 & 0x0000) ^ (r0 & 0x0004));
                    subscore[13] += eq_score((gln4 & 0x0040) ^ (l0 & 0x0000) ^ (r0 & 0x0040));
                    subscore[14] += eq_score((gln4 & 0x0400) ^ (l0 & 0x0000) ^ (r0 & 0x0400));
                    subscore[15] += eq_score((gln4 & 0x4000) ^ (l0 & 0x0000) ^ (r0 & 0x4000));

                    subscore[16] += eq_score((gln4 & 0x0002) ^ (l0 & 0x0002) ^ (r0 & 0x0002));
                    subscore[17] += eq_score((gln4 & 0x0020) ^ (l0 & 0x0200) ^ (r0 & 0x0020));
                    subscore[18] += eq_score((gln4 & 0x0200) ^ (l0 & 0x0020) ^ (r0 & 0x0200));
                    subscore[19] += eq_score((gln4 & 0x2000) ^ (l0 & 0x2000) ^ (r0 & 0x2000));
                }
            }

            sk_score[sk] = 0;
            for (auto &&s : subscore)
                sk_score[sk] += s * s;
        }

        std::ranges::iota(skn2_ranked, 0);
        std::ranges::sort(skn2_ranked,
                          [&](uint16_t x, uint16_t y) { return sk_score[x] > sk_score[y]; });

        if (real_key)
        {
            size_t real_skn2_rank = 0;
            for (size_t i = 0; i < skn2_ranked.size(); ++i)
                if (skn2_ranked[i] == real_skn2)
                    real_skn2_rank = i;

            std::println("Real sk[N-2] rank: {}", real_skn2_rank, SUBKEYS_N);
        }

        for (size_t j = 0; !key && j < SKN2_TRIALS; ++j)
        {
            std::print("Trying: {:04x}{:04x}\r", skn1_ranked[i], skn2_ranked[j]);
            std::fflush(stdout);
#ifdef USE_CUDA
            key = cu_tc05::crack(known_pln, known_cip, skn1_ranked[i], skn2_ranked[j], 0, 0);
#else
            key = crack(known_pln, known_cip, skn1_ranked[i], skn2_ranked[j], 0, 1ULL << 20);
#endif
        }
    }
    std::println("");

    std::println("Recovered key: {:016x}", key);
    if (real_key)
        std::println("Real key:      {:016x}", real_key);

    return real_skn1_rank;
}

uint64_t crack_cipher(std::span<MsgP> known_msg, uint64_t real_key = 0)
{
    if constexpr (ROUNDS_N == 1)
    {
        uint16_t l0 = known_msg[0].first >> 16;
        uint16_t r0 = known_msg[0].first;
        uint16_t l1 = known_msg[0].second >> 16;

        uint16_t k0 = l1 ^ tc05::sigma(tc05::sbox(l0)) ^ r0;

        std::println("Recovered key: {:04x}XXXXXXXXXXXX", k0);
        std::println("Real key:      {:016x}", real_key);
    }
    else if constexpr (ROUNDS_N == 2)
    {
        uint16_t l0 = known_msg[0].first >> 16;
        uint16_t r0 = known_msg[0].first;
        uint16_t l2 = known_msg[0].second >> 16;
        uint16_t r2 = known_msg[0].second;
        uint16_t l1 = r2;
        uint16_t r1 = l0;

        uint16_t k1 = l2 ^ tc05::sigma(tc05::sbox(l1)) ^ r1;
        uint16_t k0 = l1 ^ tc05::sigma(tc05::sbox(l0)) ^ r0;

        std::println("Recovered key: {:04x}{:04x}XXXXXXXX", k0, k1);
        std::println("Real key:      {:016x}", real_key);
    }
    else if constexpr (ROUNDS_N == 3)
    {
        uint16_t l0 = known_msg[0].first >> 16;
        uint16_t r0 = known_msg[0].first;
        uint16_t l3 = known_msg[0].second >> 16;
        uint16_t r3 = known_msg[0].second;
        uint16_t l2 = r3;
        uint16_t r1 = l0;
        uint16_t k0 = 0;
        uint16_t k1 = 0;
        uint16_t k2 = 0;
        uint64_t k = 0;

        for (uint32_t sk0 = 0, found = 0; sk0 <= SUBKEYS_N && !found; ++sk0)
        {
            k0 = sk0;
            uint16_t l1 = tc05::sigma(tc05::sbox(l0)) ^ r0 ^ k0;

            uint16_t r2 = l1;

            k1 = l2 ^ tc05::sigma(tc05::sbox(l1)) ^ r1;
            k2 = l3 ^ tc05::sigma(tc05::sbox(l2)) ^ r2;

            k = k0;
            k = k << 16 | k1;
            k = k << 16 | k2;
            k = k << 16 | 0;

            found = 1;
            for (auto &&[pln, cip] : known_msg)
                if (tc05::enc(pln, k, ROUNDS_N) != cip)
                {
                    found = 0;
                    break;
                }
        }

        std::println("Recovered key: {:016x}", k);
        std::println("Real key:      {:016x}", real_key);
    }
    else if constexpr (ROUNDS_N == 4)
    {
        std::array<int, SUBKEYS_N> sk0_score{};

        for (uint32_t sk = 0; sk < SUBKEYS_N; ++sk)
        {
            for (auto [pln, cip] : known_msg)
            {
                uint16_t l0 = pln >> 16;
                uint16_t r0 = pln;
                uint16_t l4 = cip >> 16;
                uint16_t r4 = cip;
                uint16_t gl1 = tc05::sigma(tc05::sbox(l0)) ^ sk ^ r0;
                uint16_t l2 = l4 ^ tc05::sigma(tc05::sbox(r4));
                uint16_t mid = tc05::sigma(tc05::sbox(gl1)) ^ l0 ^ l2;

                sk0_score[sk] += 2 * hp(mid) - 1;
            }
            sk0_score[sk] *= sk0_score[sk];
        }

        int best_score_0 = *std::ranges::max_element(sk0_score);


        uint16_t l0 = known_msg[0].first >> 16;
        uint16_t r0 = known_msg[0].first;
        uint16_t l4 = known_msg[0].second >> 16;
        uint16_t r4 = known_msg[0].second;
        uint16_t l3 = r4;
        uint16_t r1 = l0;
        uint16_t k0 = 0;
        uint16_t k1 = 0;
        uint16_t k2 = 0;
        uint16_t k3 = 0;
        uint64_t k = 0;

        for (uint32_t sk0 = 0, found = 0; !found && sk0 <= SUBKEYS_N; ++sk0)
        {
            if (sk0_score[sk0] < best_score_0)
                continue;
            k0 = sk0;

            uint16_t l1 = tc05::sigma(tc05::sbox(l0)) ^ r0 ^ k0;
            uint16_t r2 = l1;

            for (uint32_t sk1 = 0; !found && sk1 <= SUBKEYS_N; ++sk1)
            {
                k1 = sk1;

                uint16_t l2 = tc05::sigma(tc05::sbox(l1)) ^ r1 ^ k1;

                uint16_t r3 = l2;
                k2 = l3 ^ tc05::sigma(tc05::sbox(l2)) ^ r2;

                k3 = l4 ^ tc05::sigma(tc05::sbox(l3)) ^ r3;

                k = k0;
                k = k << 16 | k1;
                k = k << 16 | k2;
                k = k << 16 | k3;

                found = 1;
                for (auto &&[pln, cip] : known_msg)
                    if (tc05::enc(pln, k, ROUNDS_N) != cip)
                    {
                        found = 0;
                        break;
                    }
            }
        }

        std::println("Recovered key: {:016x}", k);
        std::println("Real key:      {:016x}", real_key);
    }
    else
    {
        return linear_attack(known_msg, real_key);
    }

    return 0;
}

void test_enc_dec()
{
    std::println("======== TEST ENC ========");

    uint32_t msg = 0x12345678;
    uint64_t key = 0x1234567890ABCDEF;

#ifdef USE_CUDA
    uint32_t cip_cpu = tc05::enc(msg, key);
    uint32_t cip_gpu = cu_tc05::test_enc(msg, key);

    std::println("enc({:08x}, {:016x}) = {:08x}", msg, key, cip_cpu);
    std::println("cuda_enc({:08x}, {:016x}) = {:08x}", msg, key, cip_gpu);
    assert(cip_cpu == cip_gpu);

    uint32_t dec_cpu = tc05::dec(cip_cpu, key);
    uint16_t sk[4];

    tc05::sched(sk, key, ROUNDS_N - 4);

    uint64_t ekey = static_cast<uint64_t>(sk[(ROUNDS_N - 1) & 3]) << 48;

    ekey |= static_cast<uint64_t>(sk[(ROUNDS_N - 2) & 3]) << 32;
    ekey |= static_cast<uint64_t>(sk[(ROUNDS_N - 3) & 3]) << 16;
    ekey |= static_cast<uint64_t>(sk[(ROUNDS_N - 4) & 3]) << 00;

    uint32_t dec_gpu = cu_tc05::test_dec(cip_gpu, ekey);

    std::println("dec({:08x}, {:016x}) = {:08x}", cip_cpu, key, dec_cpu);
    std::println("cuda_dec({:08x}, {:016x}) = {:08x}", cip_gpu, key, dec_gpu);

    assert(dec_cpu == dec_gpu);

    std::println("==========================\n");
#else
    uint32_t cip_cpu = tc05::enc(msg, key);

    std::println("enc({:08x}, {:016x}) = {:08x}", msg, key, cip_cpu);

    uint32_t dec_cpu1 = tc05::dec(cip_cpu, key);
    uint16_t sk[4];

    tc05::sched(sk, key, ROUNDS_N - 4);

    uint64_t ekey = static_cast<uint64_t>(sk[(ROUNDS_N - 1) & 3]) << 48;
    ekey |= static_cast<uint64_t>(sk[(ROUNDS_N - 2) & 3]) << 32;
    ekey |= static_cast<uint64_t>(sk[(ROUNDS_N - 3) & 3]) << 16;
    ekey |= static_cast<uint64_t>(sk[(ROUNDS_N - 4) & 3]) << 00;

    uint32_t dec_cpu2 = tc05::dec_endkey(cip_cpu, ekey);

    std::println("dec({:08x}, {:016x}) = {:08x}", cip_cpu, key, dec_cpu1);
    std::println("dec_endkey({:08x}, {:016x}) = {:08x}", cip_cpu, ekey, dec_cpu2);

    assert(dec_cpu1 == dec_cpu2);

    std::println("==========================\n");

#endif
}

int main()
{
    uint32_t seed = std::random_device{}();
    std::mt19937_64 prng{3993710677};

    std::println("RNG seed: {}", seed);

    test_enc_dec();

    LAT lat{build_lat()};
    std::println("Linear Approximation Table:");
    print_table(lat, 16, 16);
    std::println("");

    BTab btab{build_btab(lat)};
    std::println("Bias Table:");
    print_table(btab, 16, 16);
    std::println("");

    std::println("sigma(x):");
    for (uint8_t x = 0; x < 16; ++x)
        std::println("{:x} -> {:x}", x, tc05::sigma(x));

    std::println("\n======== Beginning experiments ========\n");
    uint64_t rank_avg = 0;
    for (size_t i = 0; i < EXPERIMENTS_N; ++i)
    {
        std::println("\n---- Experiment {}/{} ----\n", i + 1, EXPERIMENTS_N);

        uint64_t real_key = prng() & 0xFFFF'FFFF'FFFF'FFFF;
        std::vector<MsgP> known_msg(KNOWN_MSG_N);
        for (size_t i = 0; i < known_msg.size(); ++i)
        {
            known_msg[i].first = prng();
            known_msg[i].second = tc05::enc(known_msg[i].first, real_key, ROUNDS_N);
        }

        rank_avg += crack_cipher(known_msg, real_key);
    }
    std::println("Average rank: {}", (double)rank_avg / EXPERIMENTS_N);

    /*
    std::println("\n======== Beginning Real attack ========\n");
    std::println("Loading plaintexts/ciphertext pairs...\n");
    std::vector<MsgP> known_msg;
    std::ifstream database{DB_FNAME};

    if (!database)
    {
        std::println(stderr, "Error opening database file");
        exit(EXIT_FAILURE);
    }

    database >> std::hex;
    while (database)
    {
        MsgP pair;

        database >> pair.first;
        database >> pair.second;

        known_msg.emplace_back(pair);
    }
    if (known_msg.size() > KNOWN_MSG_N)
        known_msg.resize(KNOWN_MSG_N);

    std::println("Loaded {} pairs", known_msg.size());

    std::println("Sample data:");
    for (size_t i = 0; i < 4; ++i)
        std::println("{:08x}, {:08x}", known_msg[i].first, known_msg[i].second);

    std::println("Starting attack...");

    crack_cipher(known_msg);*/


    return 0;
}
