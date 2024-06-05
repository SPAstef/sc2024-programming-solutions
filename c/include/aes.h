#pragma once

#include <stdint.h>
#include "fq/fq_mat_types.h"

enum { AES_BLOCK_SIZE = 16U };

fq_mat_t aes128_frombytes(const uint8_t *data);

void aes128_tobytes(uint8_t *data, fq_mat_t x);

fq_mat_t aes128_addroundkey(fq_mat_t blk, fq_mat_t rk);

fq_t aes128_sbox(fq_t x);

fq_mat_t aes128_subbytes(fq_mat_t blk);

fq_mat_t aes128_shiftrows(fq_mat_t blk);

fq_mat_t aes128_mixcolumns(fq_mat_t blk);

fq_mat_t aes128_schedule(fq_mat_t key, size_t round);

void aes128_encrypt_block(uint8_t *cip, const uint8_t *key, const uint8_t *msg);
