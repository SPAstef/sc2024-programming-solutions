#include "cu_tc05.hpp"

#include <chrono>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <iomanip>
#include <iostream>

namespace cu::crypto::tc05
{
    static constexpr uint32_t GRDSZ = 1 << 16;
    static constexpr uint32_t BLKSZ = 512;
    static constexpr uint32_t VECSZ = 16;
    static constexpr size_t KNOWN_MSG_N = 16;

    __constant__ uint32_t known_msg_gpu[KNOWN_MSG_N];
    __constant__ uint32_t known_cip_gpu[KNOWN_MSG_N];
    __constant__ uint64_t base_key_gpu;
    __managed__ uint64_t guess_key_shr;

    namespace device
    {
        __device__ void static inline cmov(int c, uint64_t *x, uint64_t y)
        {
#if 1
            if (c)
                *x = y;
#else
            asm volatile inline("{\n\t"                       //
                                " .reg .pred %p;\n\t"         // declare p-register
                                " setp.eq.s32 %p, %1, 1;\n\t" // c == 1?
                                " @%p st.u64 [%0], %2;\n\t"   // set *x to y if true
                                "}"                           //
                                ::"l"(x),
                                "r"(c), "l"(y)
                                : "memory");
#endif
        }

        __device__ static inline uint16_t sigma(uint16_t word)
        {
            uint16_t new_word = 0;

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

            return new_word;
        }

        __device__ static inline uint32_t sigma_par(uint32_t word)
        {
            uint32_t new_word = 0;

            new_word |= (word & 0xC00CC00C) >> 1;
            new_word |= (word & 0x00200020) >> 2;
            new_word |= (word & 0x00100010) >> 4;
            new_word |= (word & 0x0C000C00) >> 5;
            new_word |= (word & 0x20002000) >> 6;
            new_word |= (word & 0x10001000) >> 8;

            new_word |= (word & 0x00C000C0) << 3;
            new_word |= (word & 0x01000100) << 4;
            new_word |= (word & 0x02000200) << 6;
            new_word |= (word & 0x00010001) << 8;
            new_word |= (word & 0x00020002) << 10;

            return new_word;
        }

        __device__ static inline uchar4 sigma_v2(uchar4 m)
        {
            uchar4 c;

            c.x = (m.y & 2) << 2;
            c.x |= m.y & 1;
            c.x |= (m.x >> 1) & 6;

            c.y = (m.w & 2) << 2;
            c.y |= m.w & 1;
            c.y |= (m.z >> 1) & 6;

            c.z = (m.x & 2) << 2;
            c.z |= m.x & 1;
            c.z |= (m.y >> 1) & 6;

            c.w = (m.z & 2) << 2;
            c.w |= m.z & 1;
            c.w |= (m.w >> 1) & 6;

            return c;
        }

        __device__ static inline uint16_t sbox(uint16_t m)
        {
            static constexpr uint16_t sbox[16] = {0xE, 0xB, 0x4, 0x6, 0xA, 0xD, 0x7, 0x0,
                                                  0x3, 0x8, 0xF, 0xC, 0x5, 0x9, 0x1, 0x2};
            uint16_t p = sbox[m & 0xF];

            p |= sbox[m >> 4 & 0xF] << 4;
            p |= sbox[m >> 8 & 0xF] << 8;
            p |= sbox[m >> 12] << 12;

            return p;
        }

        __device__ static inline uint32_t sbox_par(uint32_t m)
        {
            static constexpr uint16_t sbox[16] = {0xE, 0xB, 0x4, 0x6, 0xA, 0xD, 0x7, 0x0,
                                                  0x3, 0x8, 0xF, 0xC, 0x5, 0x9, 0x1, 0x2};
            uint32_t p = sbox[m & 0xF];

            p |= sbox[m >> 4 & 0xF] << 4;
            p |= sbox[m >> 8 & 0xF] << 8;
            p |= sbox[m >> 12 & 0xF] << 12;
            p |= sbox[m >> 16 & 0xF] << 16;
            p |= sbox[m >> 20 & 0xF] << 20;
            p |= sbox[m >> 24 & 0xF] << 24;
            p |= sbox[m >> 28 & 0xF] << 28;

            return p;
        }

        template<uint32_t rounds>
        __device__ static inline uint32_t enc(uint32_t m, uint64_t k)
        {
            uint16_t sk[4] = {static_cast<uint16_t>(k >> 48), static_cast<uint16_t>(k >> 32),
                              static_cast<uint16_t>(k >> 16), static_cast<uint16_t>(k)};
            uint16_t l = m >> 16;
            uint16_t r = m;

#pragma unroll(rounds)
            for (uint32_t i = 0; i < rounds; ++i)
            {
                uint16_t t = l;
                l = sigma(sbox(l)) ^ r ^ sk[i & 3];
                r = t;

                sk[i & 3] ^= sk[(i - 1) & 3];
                sk[i & 3] ^= sigma(sk[(i - 2) & 3]);
                sk[i & 3] ^= 0xC;
            }

            return (uint32_t)l << 16 | r;
        }

        template<uint32_t rounds>
        __device__ static inline uint32_t dec(uint32_t m, uint64_t last_key)
        {
            uint16_t sk[4];
            uint16_t l = m >> 16;
            uint16_t r = m;

            sk[(rounds - 1) & 3] = last_key >> 48;
            sk[(rounds - 2) & 3] = last_key >> 32;
            sk[(rounds - 3) & 3] = last_key >> 16;
            sk[(rounds - 4) & 3] = last_key >> 0;


#pragma unroll
            for (uint32_t i = rounds - 1; i >= 4; --i)
            {
                uint16_t t = r;
                r = sbox(r);
                r = sigma(r);
                r ^= l;
                r ^= sk[i & 3];
                l = t;

                sk[i & 3] ^= sk[(i - 1) & 3];
                sk[i & 3] ^= sigma(sk[(i - 2) & 3]);
                sk[i & 3] ^= 0xC;
            }

#pragma unroll
            for (uint32_t i = 3; i != ~0U; --i)
            {
                uint16_t t = r;
                r = sbox(r);
                r = sigma(r);
                r ^= l;
                r ^= sk[i & 3];
                l = t;
            }

            return (uint32_t)l << 16 | r;
        }

        template<uint32_t rounds>
        __device__ static inline uint64_t dec_par(uint32_t m1, uint32_t m2, uint64_t last_key)
        {
            uint32_t sk[4];
            uint32_t l = (m1 & 0xFFFF0000) | (m2 >> 16);
            uint32_t r = (m1 << 16) | (m2 & 0x0000FFFF);
            uint32_t t;

            sk[(rounds - 1) & 3] = (last_key >> 48) & 0xFFFF;
            sk[(rounds - 2) & 3] = (last_key >> 32) & 0xFFFF;
            sk[(rounds - 3) & 3] = (last_key >> 16) & 0xFFFF;
            sk[(rounds - 4) & 3] = (last_key >> 0) & 0xFFFF;

            sk[0] |= sk[0] << 16;
            sk[1] |= sk[1] << 16;
            sk[2] |= sk[2] << 16;
            sk[3] |= sk[3] << 16;

#pragma unroll
            for (uint32_t i = rounds - 1; i >= 4; --i)
            {
                t = r;
                r = sbox_par(r);
                r = sigma_par(r);
                r ^= l;
                r ^= sk[i & 3];
                l = t;

                sk[i & 3] ^= sk[(i - 1) & 3];
                sk[i & 3] ^= sigma_par(sk[(i - 2) & 3]);
                sk[i & 3] ^= 0x000C000C;
            }

#pragma unroll
            for (uint32_t i = 3; i != ~0U; --i)
            {
                t = r;
                r = sbox_par(r);
                r = sigma_par(r);
                r ^= l;
                r ^= sk[i & 3];
                l = t;
            }

            m1 = (l & 0xFFFF0000) | (r >> 16);
            m2 = (l << 16) | (r & 0x0000FFFF);

            return (uint64_t)m1 << 32 | m2;
        }

        __device__ static inline uint32_t dec8(uint32_t m, uint64_t last_key)
        {
            uint16_t sk[4];
            uint16_t t;
            uint16_t l = m >> 16;
            uint16_t r = m;

            sk[3] = last_key >> 48;
            sk[2] = last_key >> 32;
            sk[1] = last_key >> 16;
            sk[0] = last_key >> 0;

            t = r;
            r = sbox(r);
            r = sigma(r);
            r ^= l;
            r ^= sk[3];
            l = t;
            sk[3] ^= sk[2];
            sk[3] ^= sigma(sk[1]);
            sk[3] ^= 0xC;

            t = r;
            r = sbox(r);
            r = sigma(r);
            r ^= l;
            r ^= sk[2];
            l = t;
            sk[2] ^= sk[1];
            sk[2] ^= sigma(sk[0]);
            sk[2] ^= 0xC;

            t = r;
            r = sbox(r);
            r = sigma(r);
            r ^= l;
            r ^= sk[1];
            l = t;
            sk[1] ^= sk[0];
            sk[1] ^= sigma(sk[3]);
            sk[1] ^= 0xC;

            t = r;
            r = sbox(r);
            r = sigma(r);
            r ^= l;
            r ^= sk[0];
            l = t;
            sk[0] ^= sk[3];
            sk[0] ^= sigma(sk[2]);
            sk[0] ^= 0xC;

            t = r;
            r = sbox(r);
            r = sigma(r);
            r ^= l;
            r ^= sk[3];
            l = t;

            t = r;
            r = sbox(r);
            r = sigma(r);
            r ^= l;
            r ^= sk[2];
            l = t;

            t = r;
            r = sbox(r);
            r = sigma(r);
            r ^= l;
            r ^= sk[1];
            l = t;

            t = r;
            r = sbox(r);
            r = sigma(r);
            r ^= l;
            r ^= sk[0];

            return ((uint32_t)t << 16) | r;
        }

        __device__ static inline uint32_t dec8_v2(uint32_t m, uint64_t last_key)
        {
            static constexpr uint8_t sbox[16] = {0xE, 0xB, 0x4, 0x6, 0xA, 0xD, 0x7, 0x0,
                                                 0x3, 0x8, 0xF, 0xC, 0x5, 0x9, 0x1, 0x2};

            uint32_t hk = last_key >> 32;
            uint32_t lk = last_key;

            uchar4 sk[4] = {
                {
                    static_cast<uint8_t>((lk >> 12) & 0xF),
                    static_cast<uint8_t>((lk >> 8) & 0xF),
                    static_cast<uint8_t>((lk >> 4) & 0xF),
                    static_cast<uint8_t>((lk >> 0) & 0xF),
                },
                {
                    static_cast<uint8_t>((lk >> 28) & 0xF),
                    static_cast<uint8_t>((lk >> 24) & 0xF),
                    static_cast<uint8_t>((lk >> 20) & 0xF),
                    static_cast<uint8_t>((lk >> 16) & 0xF),
                },
                {
                    static_cast<uint8_t>((hk >> 12) & 0xF),
                    static_cast<uint8_t>((hk >> 8) & 0xF),
                    static_cast<uint8_t>((hk >> 4) & 0xF),
                    static_cast<uint8_t>((hk >> 0) & 0xF),
                },
                {
                    static_cast<uint8_t>((hk >> 28) & 0xF),
                    static_cast<uint8_t>((hk >> 24) & 0xF),
                    static_cast<uint8_t>((hk >> 20) & 0xF),
                    static_cast<uint8_t>((hk >> 16) & 0xF),
                },
            };

            uchar4 t;

            uchar4 l = {
                static_cast<uint8_t>((m >> 28) & 0xF),
                static_cast<uint8_t>((m >> 24) & 0xF),
                static_cast<uint8_t>((m >> 20) & 0xF),
                static_cast<uint8_t>((m >> 16) & 0xF),
            };

            uchar4 r = {
                static_cast<uint8_t>((m >> 12) & 0xF),
                static_cast<uint8_t>((m >> 8) & 0xF),
                static_cast<uint8_t>((m >> 4) & 0xF),
                static_cast<uint8_t>((m >> 0) & 0xF),
            };

            {
                t = r;
                r.x = sbox[r.x];
                r.y = sbox[r.y];
                r.z = sbox[r.z];
                r.w = sbox[r.w];
                r = sigma_v2(r);
                r.x ^= l.x;
                r.y ^= l.y;
                r.z ^= l.z;
                r.w ^= l.w;
                r.x ^= sk[3].x;
                r.y ^= sk[3].y;
                r.z ^= sk[3].z;
                r.w ^= sk[3].w;
                l = t;

                sk[3].x ^= sk[2].x;
                sk[3].y ^= sk[2].y;
                sk[3].z ^= sk[2].z;
                sk[3].w ^= sk[2].w;
                t = sigma_v2(sk[1]);
                sk[3].x ^= t.x;
                sk[3].y ^= t.y;
                sk[3].z ^= t.z;
                sk[3].w ^= t.w;
                sk[3].w ^= 0xC;
            }

            {
                t = r;
                r.x = sbox[r.x];
                r.y = sbox[r.y];
                r.z = sbox[r.z];
                r.w = sbox[r.w];
                r = sigma_v2(r);
                r.x ^= l.x;
                r.y ^= l.y;
                r.z ^= l.z;
                r.w ^= l.w;
                r.x ^= sk[2].x;
                r.y ^= sk[2].y;
                r.z ^= sk[2].z;
                r.w ^= sk[2].w;
                l = t;

                sk[2].x ^= sk[1].x;
                sk[2].y ^= sk[1].y;
                sk[2].z ^= sk[1].z;
                sk[2].w ^= sk[1].w;
                t = sigma_v2(sk[0]);
                sk[2].x ^= t.x;
                sk[2].y ^= t.y;
                sk[2].z ^= t.z;
                sk[2].w ^= t.w;
                sk[2].w ^= 0xC;
            }

            {
                t = r;
                r.x = sbox[r.x];
                r.y = sbox[r.y];
                r.z = sbox[r.z];
                r.w = sbox[r.w];
                r = sigma_v2(r);
                r.x ^= l.x;
                r.y ^= l.y;
                r.z ^= l.z;
                r.w ^= l.w;
                r.x ^= sk[1].x;
                r.y ^= sk[1].y;
                r.z ^= sk[1].z;
                r.w ^= sk[1].w;
                l = t;

                sk[1].x ^= sk[0].x;
                sk[1].y ^= sk[0].y;
                sk[1].z ^= sk[0].z;
                sk[1].w ^= sk[0].w;
                t = sigma_v2(sk[3]);
                sk[1].x ^= t.x;
                sk[1].y ^= t.y;
                sk[1].z ^= t.z;
                sk[1].w ^= t.w;
                sk[1].w ^= 0xC;
            }

            {
                t = r;
                r.x = sbox[r.x];
                r.y = sbox[r.y];
                r.z = sbox[r.z];
                r.w = sbox[r.w];
                r = sigma_v2(r);
                r.x ^= l.x;
                r.y ^= l.y;
                r.z ^= l.z;
                r.w ^= l.w;
                r.x ^= sk[0].x;
                r.y ^= sk[0].y;
                r.z ^= sk[0].z;
                r.w ^= sk[0].w;
                l = t;

                sk[0].x ^= sk[3].x;
                sk[0].y ^= sk[3].y;
                sk[0].z ^= sk[3].z;
                sk[0].w ^= sk[3].w;
                t = sigma_v2(sk[2]);
                sk[0].x ^= t.x;
                sk[0].y ^= t.y;
                sk[0].z ^= t.z;
                sk[0].w ^= t.w;
                sk[0].w ^= 0xC;
            }

            {
                t = r;
                r.x = sbox[r.x];
                r.y = sbox[r.y];
                r.z = sbox[r.z];
                r.w = sbox[r.w];
                r = sigma_v2(r);
                r.x ^= l.x;
                r.y ^= l.y;
                r.z ^= l.z;
                r.w ^= l.w;
                r.x ^= sk[3].x;
                r.y ^= sk[3].y;
                r.z ^= sk[3].z;
                r.w ^= sk[3].w;
                l = t;
            }

            {
                t = r;
                r.x = sbox[r.x];
                r.y = sbox[r.y];
                r.z = sbox[r.z];
                r.w = sbox[r.w];
                r = sigma_v2(r);
                r.x ^= l.x;
                r.y ^= l.y;
                r.z ^= l.z;
                r.w ^= l.w;
                r.x ^= sk[2].x;
                r.y ^= sk[2].y;
                r.z ^= sk[2].z;
                r.w ^= sk[2].w;
                l = t;
            }

            {
                t = r;
                r.x = sbox[r.x];
                r.y = sbox[r.y];
                r.z = sbox[r.z];
                r.w = sbox[r.w];
                r = sigma_v2(r);
                r.x ^= l.x;
                r.y ^= l.y;
                r.z ^= l.z;
                r.w ^= l.w;
                r.x ^= sk[1].x;
                r.y ^= sk[1].y;
                r.z ^= sk[1].z;
                r.w ^= sk[1].w;
                l = t;
            }

            {
                t = r;
                r.x = sbox[r.x];
                r.y = sbox[r.y];
                r.z = sbox[r.z];
                r.w = sbox[r.w];
                r = sigma_v2(r);
                r.x ^= l.x;
                r.y ^= l.y;
                r.z ^= l.z;
                r.w ^= l.w;
                r.x ^= sk[0].x;
                r.y ^= sk[0].y;
                r.z ^= sk[0].z;
                r.w ^= sk[0].w;
            }

            uint32_t c = r.w;

            c |= static_cast<uint32_t>(r.z) << 4;
            c |= static_cast<uint32_t>(r.y) << 8;
            c |= static_cast<uint32_t>(r.x) << 12;
            c |= static_cast<uint32_t>(t.w) << 16;
            c |= static_cast<uint32_t>(t.z) << 20;
            c |= static_cast<uint32_t>(t.y) << 24;
            c |= static_cast<uint32_t>(t.x) << 28;

            return c;
        }

        template<uint32_t rounds>
        __global__ void test_enc(uint32_t msg, uint64_t key, uint32_t *cip)
        {
            *cip = enc<rounds>(msg, key);
        }

        template<uint32_t rounds>
        __global__ void test_dec(uint32_t cip, uint64_t last_key, uint32_t *msg)
        {
            *msg = dec<rounds>(cip, last_key);
        }

        template<uint32_t rounds>
        __global__ void crack_enc(uint64_t off)
        {
            // concatenation of the 4 last subkeys, must be de-scheduled
            uint64_t key = base_key_gpu;
            uint32_t id = (blockIdx.x * blockDim.x + threadIdx.x) * VECSZ;
            //uint64_t m12 = (uint64_t)known_msg_gpu[0] << 32 | known_msg_gpu[1];

            off += id;

#pragma unroll
            for (uint32_t i = 0; i < VECSZ; ++i)
            {
                key &= 0xFFFFFFFF00000000;
                key |= (off + i);

                bool flag = dec<rounds>(known_cip_gpu[0], key) == known_msg_gpu[0];

                if (flag) [[unlikely]]
                {
                    flag = dec<rounds>(known_cip_gpu[1], key) == known_msg_gpu[1];
                    if (flag) [[unlikely]]
                        guess_key_shr = key;
                }
            }
        }

        __global__ void crack_enc8(uint64_t off)
        {
            // concatenation of the 4 last subkeys, must be de-scheduled
            bool flag;
            uint64_t key = base_key_gpu;

            off += (blockIdx.x * blockDim.x + threadIdx.x) * VECSZ;
            key |= off;

            flag = (dec8(known_cip_gpu[0], key) == known_msg_gpu[0]);
            if (flag)
            {
                flag &= dec8(known_cip_gpu[1], key) == known_msg_gpu[1];
                if (flag)
                    guess_key_shr = key;
            }

            ++key;
            flag = (dec8(known_cip_gpu[0], key) == known_msg_gpu[0]);
            if (flag)
            {
                flag &= dec8(known_cip_gpu[1], key) == known_msg_gpu[1];
                if (flag)
                    guess_key_shr = key;
            }

            ++key;
            flag = (dec8(known_cip_gpu[0], key) == known_msg_gpu[0]);
            if (flag)
            {
                flag &= dec8(known_cip_gpu[1], key) == known_msg_gpu[1];
                if (flag)
                    guess_key_shr = key;
            }

            ++key;
            flag = (dec8(known_cip_gpu[0], key) == known_msg_gpu[0]);
            if (flag)
            {
                flag &= dec8(known_cip_gpu[1], key) == known_msg_gpu[1];
                if (flag)
                    guess_key_shr = key;
            }
        }
    } // namespace device

    template<uint32_t rounds>
    uint32_t test_enc(uint32_t msg, uint64_t key)
    {
        uint32_t cip;
        uint32_t *d_cip;

        cudaMalloc(&d_cip, sizeof(*d_cip));
        device::test_enc<rounds><<<1, 1>>>(msg, key, d_cip);
        cudaDeviceSynchronize();
        cudaMemcpy(&cip, d_cip, sizeof(cip), cudaMemcpyDeviceToHost);
        cudaFree(d_cip);

        return cip;
    }

    uint32_t test_enc(uint32_t msg, uint64_t key)
    {
        return test_enc<ROUNDS_N>(msg, key);
    }

    template<uint32_t rounds>
    uint32_t test_dec(uint32_t cip, uint64_t last_key)
    {
        uint32_t msg;
        uint32_t *d_msg;

        cudaMalloc(&d_msg, sizeof(*d_msg));
        device::test_dec<rounds><<<1, 1>>>(cip, last_key, d_msg);
        cudaDeviceSynchronize();
        cudaMemcpy(&msg, d_msg, sizeof(msg), cudaMemcpyDeviceToHost);
        cudaFree(d_msg);

        return msg;
    }

    uint32_t test_dec(uint32_t cip, uint64_t last_key)
    {
        return test_dec<ROUNDS_N>(cip, last_key);
    }

    template<uint32_t rounds>
    uint64_t crack(std::span<uint32_t> msg, std::span<uint32_t> cip, uint16_t skn1, uint16_t skn2,
                   uint64_t off, size_t watch)
    {
        using namespace std::chrono;
        using clk = high_resolution_clock;
        static constexpr uint64_t RECOVER_SPACE = 1ULL << 32;

        if (test_enc<rounds>(msg[0], 0) == cip[0])
            return 0;

        cudaStream_t compute_stream;

        cudaStreamCreate(&compute_stream);

        cudaMemcpyToSymbol(known_msg_gpu, msg.data(), KNOWN_MSG_N * sizeof(*known_msg_gpu));
        cudaMemcpyToSymbol(known_cip_gpu, cip.data(), KNOWN_MSG_N * sizeof(*known_cip_gpu));
        uint64_t base_key = (uint64_t)skn1 << 48 | (uint64_t)skn2 << 32;
        cudaMemcpyToSymbol(base_key_gpu, &base_key, sizeof(base_key));

        std::cout << std::left;

        auto start = clk::now();
        for (size_t i = 0; !guess_key_shr && off < RECOVER_SPACE; ++i, off += GRDSZ * BLKSZ * VECSZ)
        {
            device::crack_enc<rounds><<<GRDSZ, BLKSZ, 0, compute_stream>>>(off);
            if (watch && !(i % watch))
            {
                auto elap = clk::now() - start;
                auto elap_ns = duration_cast<nanoseconds>(elap).count();
                auto elap_sec = duration_cast<seconds>(elap).count();

                std::cout << std::setw(20) << off << '\t' << elap_sec << " sec. "
                          << "(" << ((double)off / elap_sec) << " enc/s)\r";
                std::cout.flush();
            }
        }
        if (watch)
            std::cout << '\n';

        std::cout << std::right;
        cudaDeviceSynchronize();
        cudaStreamDestroy(compute_stream);

        uint64_t key = guess_key_shr;

        guess_key_shr = 0;

        if (key)
        {
            uint16_t sk[4];

            sk[(rounds - 1) & 3] = key >> 48;
            sk[(rounds - 2) & 3] = key >> 32;
            sk[(rounds - 3) & 3] = key >> 16;
            sk[(rounds - 4) & 3] = key >> 0;

            for (uint32_t i = rounds - 1; i >= 4; --i)
                ::crypto::tc05::next_key(sk, i);

            key = sk[3];
            key |= static_cast<uint64_t>(sk[2]) << 16;
            key |= static_cast<uint64_t>(sk[1]) << 32;
            key |= static_cast<uint64_t>(sk[0]) << 48;
        }

        return key;
    }

    uint64_t crack(std::span<uint32_t> msg, std::span<uint32_t> cip, uint16_t skn2, uint16_t skn1,
                   uint64_t off, size_t watch)
    {
        return crack<ROUNDS_N>(msg, cip, skn2, skn1, off, watch);
    }

} // namespace cu::crypto::tc05
