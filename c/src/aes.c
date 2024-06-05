#include "aes.h"
#include "fq/fq.h"
#include "fq/fq_mat.h"
#include "fq/fq_poly.h"
#include "fq/fq_vec.h"
#include "string_utils.h"
#include "zp/zp.h"
#include "zp/zp_poly.h"
#include "zp/zp_vec.h"

enum
{
    AES_DEBUG_PRINT = 0U, // AES print debug information
    AES_P = 2U,           // AES prime field characteristic
    AES_K = 8U,           // AES prime field extension degree
    AES_ROWS = 4U,        // AES block number of rows
    AES_COLS = 4U,        // AES block number of columns
    AES_ROUNDS = 10U,     // AES number of rounds
    AES_ALPHA = 0b10U,    // AES imaginary unit
};

// AES irreducible polynomial: x^8 + x^4 + x^3 + x + 1 (over Z_2[x])
static const zp_poly_t AES_R = {.c = (zp_t[AES_K + 1]){{1}, {1}, {0}, {1}, {1}, {0}, {0}, {0}, {1}},
                                .n = AES_K + 1};

/*
// AES S-box circulant matrix (over Z_2)
static const zp_vec_t SUBBYTES_MAT = {.c = (zp_t[AES_K]){{1}, {0}, {0}, {0}, {1}, {1}, {1}, {1}},
                                      .n = AES_K};

// AES S-box affine transformation vector (over Z_2)
static const zp_vec_t SUBBYTES_AFF = {.c = (zp_t[AES_K]){{1}, {1}, {0}, {0}, {0}, {1}, {1}, {0}},
                                      .n = AES_K};
*/
// AES S-box affine transformation slope: x^4 + x^3 + x^2 + x + 1  (over Z_2[x])
static const zp_poly_t SUBBYTES_AFF_M = {.c = (zp_t[5]){{1}, {1}, {1}, {1}, {1}}, .n = 5};

// AES S-box affine transformation reduction polynomial: x^8 + 1 (over Z_2[x])
static zp_poly_t SUBBYTES_AFF_R = {
    .c = (zp_t[AES_K + 1]){{1}, {0}, {0}, {0}, {0}, {0}, {0}, {0}, {1}},
    .n = AES_K + 1};

// AES S-box affine transformation intercept: a^6 + a^5 + a + 1 (over Z_2[x])
static const fq_t SUBBYTES_AFF_Q = {
    .v = {.c = (zp_t[7]){{1}, {1}, {0}, {0}, {0}, {1}, {1}}, .n = 7}};

// AES rotation reduction polynomial: x^4 + 1 (over Z_2^8[x])
static const fq_poly_t AES_ROT_R = {
    .c =
        (fq_t[5]){
            {.v = {.c = (zp_t[1]){{1}}, .n = 1}},
            {.v = {.c = (zp_t[1]){{0}}, .n = 1}},
            {.v = {.c = (zp_t[1]){{0}}, .n = 1}},
            {.v = {.c = (zp_t[1]){{0}}, .n = 1}},
            {.v = {.c = (zp_t[1]){{1}}, .n = 1}},
        },
    .n = 5,
};

// AES shiftrows rotation polynomial: x (over Z_2^8[x])
static const fq_poly_t AES_SHIFTROWS_POLY = {
    .c =
        (fq_t[2]){
            {.v = {.c = (zp_t[1]){{0}}, .n = 1}},
            {.v = {.c = (zp_t[1]){{1}}, .n = 1}},
        },
    .n = 2,
};

// AES mixcolumns polynomial: (a + 1)x^3 + x^2 + x + a (over Z_2^8[x])
static const fq_poly_t AES_MIXCOLUMNS_POLY = {
    .c =
        (fq_t[4]){
            {.v = {.c = (zp_t[2]){{0}, {1}}, .n = 2}},
            {.v = {.c = (zp_t[1]){{1}}, .n = 1}},
            {.v = {.c = (zp_t[1]){{1}}, .n = 1}},
            {.v = {.c = (zp_t[2]){{1}, {1}}, .n = 2}},
        },
    .n = 4,
};

fq_mat_t aes128_frombytes(const uint8_t *data)
{
    fq_mat_t x = fq_mat_new_empty(AES_ROWS, AES_COLS);

    for (size_t i = 0; i < AES_ROWS; ++i)
        for (size_t j = 0; j < AES_COLS; ++j)
            x.c[j * AES_COLS + i] = fq_from_int(data[i * AES_COLS + j], AES_P);

    return x;
}

void aes128_tobytes(uint8_t *data, fq_mat_t x)
{
    for (size_t i = 0; i < AES_ROWS; ++i)
        for (size_t j = 0; j < AES_COLS; ++j)
            data[i * AES_COLS + j] = fq_to_int(x.c[j * AES_COLS + i], AES_P);
}

fq_mat_t aes128_addroundkey(fq_mat_t blk, fq_mat_t rk)
{
    return fq_mat_self_add(blk, rk, AES_P);
}

fq_t aes128_sbox(fq_t x)
{
    x = fq_self_inv(x, AES_R, AES_P);
    x.v = zp_poly_self_mul(x.v, SUBBYTES_AFF_M, AES_P);
    x.v = zp_poly_self_rem(x.v, SUBBYTES_AFF_R, AES_P);
    x = fq_self_add(x, SUBBYTES_AFF_Q, AES_P);

    return x;
}

fq_mat_t aes128_subbytes(fq_mat_t blk)
{
    //blk = fq_mat_self_hinv(blk, AES_R, AES_P);
    for (size_t i = 0; i < AES_ROWS; ++i)
        for (size_t j = 0; j < AES_COLS; ++j)
            blk.c[i * AES_COLS + j] = aes128_sbox(blk.c[i * AES_COLS + j]);

    return blk;
}

fq_mat_t aes128_shiftrows(fq_mat_t blk)
{
    for (size_t i = 0; i < AES_ROWS; ++i)
    {
        fq_poly_t t = fq_poly_from_fq_vec_copy(fq_mat_row_view(blk, i));

        for (size_t j = 0; j < AES_ROWS - i; ++j)
        {
            t = fq_poly_self_mul(t, AES_SHIFTROWS_POLY, AES_R, AES_P);
            t = fq_poly_self_rem(t, AES_ROT_R, AES_R, AES_P);
        }

        for (size_t j = 0; j < AES_COLS; ++j)
            fq_copy(&blk.c[i * AES_COLS + j], t.c[j]);

        fq_poly_del(t);
    }

    return blk;
}

fq_mat_t aes128_mixcolumns(fq_mat_t blk)
{
    blk = fq_mat_self_trans(blk);

    for (size_t i = 0; i < AES_COLS; ++i)
    {
        fq_poly_t t = fq_poly_from_fq_vec_copy(fq_mat_row_view(blk, i));

        t = fq_poly_self_mul(t, AES_MIXCOLUMNS_POLY, AES_R, AES_P);
        t = fq_poly_self_rem(t, AES_ROT_R, AES_R, AES_P);

        for (size_t j = 0; j < AES_ROWS; ++j)
            fq_copy(&blk.c[i * AES_ROWS + j], t.c[j]);

        fq_poly_del(t);
    }

    return fq_mat_self_trans(blk);
}

fq_mat_t aes128_schedule(fq_mat_t key, size_t r)
{
    key = fq_mat_self_trans(key);

    fq_poly_t t = fq_poly_from_fq_vec_copy(fq_mat_row_view(key, AES_ROWS - 1));

    for (size_t i = 1; i < AES_ROWS; ++i)
    {
        t = fq_poly_self_mul(t, AES_SHIFTROWS_POLY, AES_R, AES_P);
        t = fq_poly_self_rem(t, AES_ROT_R, AES_R, AES_P);
    }
    fq_poly_resize(&t, AES_ROWS);

    for (size_t i = 0; i < AES_ROWS; ++i)
        t.c[i] = aes128_sbox(t.c[i]);

    // round constant: alpha^r
    fq_t rc = fq_self_pow(fq_from_int(AES_ALPHA, AES_P), r, AES_R, AES_P);

    t.c[0] = fq_self_add(t.c[0], rc, AES_P);
    fq_vec_self_add(fq_mat_row_view(key, 0), fq_vec_from_fq_poly_view(t), AES_P);

    fq_del(rc);
    fq_poly_del(t);

    for (size_t i = 1; i < AES_COLS; ++i)
        for (size_t j = 0; j < AES_ROWS; ++j)
            key.c[i * AES_ROWS + j] = fq_self_add(key.c[i * AES_ROWS + j],
                                                  key.c[(i - 1) * AES_ROWS + j], AES_P);

    return fq_mat_self_trans(key);
}

void aes128_encrypt_block(uint8_t *cip_data, const uint8_t *key_data, const uint8_t *msg_data)
{
    fq_mat_t key = aes128_frombytes(key_data);
    fq_mat_t blk = aes128_frombytes(msg_data);
    uint8_t key_copy[AES_BLOCK_SIZE] = {0};


    if (AES_DEBUG_PRINT)
    {
        aes128_tobytes(key_copy, key);
        printf("key_in: ");
        hexprint(stdout, key_copy, AES_BLOCK_SIZE);
        printf("\n");
        fq_mat_print(stdout, key);
        printf("\n");
    }

    if (AES_DEBUG_PRINT)
    {
        aes128_tobytes(cip_data, blk);
        printf("blk_in: ");
        hexprint(stdout, cip_data, AES_BLOCK_SIZE);
        printf("\n");
        fq_mat_print(stdout, blk);
        printf("\n");
    }

    blk = aes128_addroundkey(blk, key);
    if (AES_DEBUG_PRINT)
    {
        aes128_tobytes(cip_data, blk);
        printf("blk0_ak: ");
        hexprint(stdout, cip_data, AES_BLOCK_SIZE);
        fq_mat_print(stdout, blk);
        printf("\n==============\n");
    }

    for (size_t i = 0; i < AES_ROUNDS - 1; ++i)
    {
        key = aes128_schedule(key, i);
        if (AES_DEBUG_PRINT)
        {
            aes128_tobytes(key_copy, key);
            printf("key%zu: ", i + 1);
            hexprint(stdout, key_copy, AES_BLOCK_SIZE);
            printf("\n");
            fq_mat_print(stdout, key);
            printf("\n");
        }

        blk = aes128_subbytes(blk);
        if (AES_DEBUG_PRINT)
        {
            aes128_tobytes(cip_data, blk);
            printf("blk%zu_sb: ", i + 1);
            hexprint(stdout, cip_data, AES_BLOCK_SIZE);
            printf("\n");
            fq_mat_print(stdout, blk);
            printf("\n");
        }

        blk = aes128_shiftrows(blk);
        if (AES_DEBUG_PRINT)
        {
            aes128_tobytes(cip_data, blk);
            printf("blk%zu_sr: ", i + 1);
            hexprint(stdout, cip_data, AES_BLOCK_SIZE);
            printf("\n");
            fq_mat_print(stdout, blk);
            printf("\n");
        }

        blk = aes128_mixcolumns(blk);
        if (AES_DEBUG_PRINT)
        {
            aes128_tobytes(cip_data, blk);
            printf("blk%zu_mc: ", i + 1);
            hexprint(stdout, cip_data, AES_BLOCK_SIZE);
            printf("\n");
            fq_mat_print(stdout, blk);
            printf("\n");
        }

        blk = aes128_addroundkey(blk, key);
        if (AES_DEBUG_PRINT)
        {
            aes128_tobytes(cip_data, blk);
            printf("blk%zu_ak: ", i + 1);
            hexprint(stdout, cip_data, AES_BLOCK_SIZE);
            printf("\n");
            fq_mat_print(stdout, blk);
            printf("\n==============\n");
        }
    }

    key = aes128_schedule(key, AES_ROUNDS - 1);
    if (AES_DEBUG_PRINT)
    {
        aes128_tobytes(key_copy, key);
        printf("key%d: ", AES_ROUNDS);
        hexprint(stdout, key_copy, AES_BLOCK_SIZE);
        printf("\n");
        fq_mat_print(stdout, key);
        printf("\n");
    }

    blk = aes128_subbytes(blk);
    if (AES_DEBUG_PRINT)
    {
        aes128_tobytes(cip_data, blk);
        printf("blk%d_sb: ", AES_ROUNDS);
        hexprint(stdout, cip_data, AES_BLOCK_SIZE);
        printf("\n");
        fq_mat_print(stdout, blk);
        printf("\n");
    }

    blk = aes128_shiftrows(blk);
    if (AES_DEBUG_PRINT)
    {
        aes128_tobytes(cip_data, blk);
        printf("blk%d_sr: ", AES_ROUNDS);
        hexprint(stdout, cip_data, AES_BLOCK_SIZE);
        printf("\n");
        fq_mat_print(stdout, blk);
        printf("\n");
    }

    blk = aes128_addroundkey(blk, key);
    if (AES_DEBUG_PRINT)
    {
        aes128_tobytes(cip_data, blk);
        printf("cip: ");
        hexprint(stdout, cip_data, AES_BLOCK_SIZE);
        printf("\n");
        fq_mat_print(stdout, blk);
        printf("\n==============\n");
    }

    aes128_tobytes(cip_data, blk);

    fq_mat_del(key);
    fq_mat_del(blk);
}
