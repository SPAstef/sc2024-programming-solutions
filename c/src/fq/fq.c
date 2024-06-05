#include "fq/fq.h"
#include "rand.h"
#include "utils.h"
#include "zp/zp.h"
#include "zp/zp_poly.h"
#include "zp/zp_vec.h"

size_t fq_digits(fq_t x)
{
    return x.v.n;
}

fq_t fq_new_empty(size_t dig)
{
    return (fq_t){zp_poly_new_empty(dig - 1)};
}

fq_t fq_new(size_t dig)
{
    return (fq_t){zp_poly_new(dig - 1)};
}

fq_t fq_new_copy(fq_t x)
{
    return (fq_t){zp_poly_new_copy(x.v)};
}

fq_t fq_new_view(fq_t x)
{
    return x;
}

void fq_del(fq_t x)
{
    zp_poly_del(x.v);
}

void fq_copy(fq_t *x, fq_t y)
{
    fq_del(*x);

    *x = fq_new_copy(y);
}

void fq_move(fq_t *x, fq_t y)
{
    fq_del(*x);

    *x = y;
}

fq_t fq_from_zp_poly_view(zp_poly_t x)
{
    return (fq_t){x};
}

fq_t fq_from_zp_poly_copy(zp_poly_t x)
{
    return fq_from_zp_poly_view(zp_poly_new_copy(x));
}

fq_t fq_from_zp_vec_view(zp_vec_t x)
{
    return fq_from_zp_poly_view(zp_poly_from_zp_vec_view(x));
}

fq_t fq_from_zp_vec_copy(zp_vec_t x)
{
    return fq_from_zp_poly_view(zp_poly_from_zp_vec_copy(x));
}

zp_poly_t fq_to_zp_poly_view(fq_t x)
{
    return zp_poly_new_view(x.v);
}

zp_poly_t fq_to_zp_poly_copy(fq_t x)
{
    return zp_poly_new_copy(x.v);
}

fq_t fq_zero()
{
    return fq_from_zp_poly_view(zp_poly_zero());
}

fq_t fq_one()
{
    return fq_from_zp_poly_view(zp_poly_one());
}

bool fq_is_zero(fq_t x)
{
    return zp_poly_is_zero(x.v);
}

bool fq_is_one(fq_t x)
{
    return zp_poly_is_one(x.v);
}

fq_t fq_self_add(fq_t x, fq_t y, uint64_t p)
{
    x.v = zp_poly_self_add(x.v, y.v, p);

    return x;
}

fq_t fq_add(fq_t x, fq_t y, uint64_t p)
{
    return fq_self_add(fq_new_copy(x), y, p);
}

fq_t fq_self_neg(fq_t x, uint64_t p)
{
    x.v = zp_poly_self_neg(x.v, p);

    return x;
}

fq_t fq_neg(fq_t x, uint64_t p)
{
    return fq_self_neg(fq_new_copy(x), p);
}

fq_t fq_self_sub(fq_t x, fq_t y, uint64_t p)
{
    x.v = zp_poly_self_sub(x.v, y.v, p);

    return x;
}

fq_t fq_sub(fq_t x, fq_t y, uint64_t p)
{
    return fq_self_sub(fq_new_copy(x), y, p);
}

fq_t fq_self_mul(fq_t x, fq_t y, zp_poly_t r, uint64_t p)
{
    zp_poly_move(&x.v, zp_poly_mul(x.v, y.v, p));
    x.v = zp_poly_self_rem(x.v, r, p);

    return x;
}

fq_t fq_mul(fq_t x, fq_t y, zp_poly_t r, uint64_t p)
{
    return fq_self_mul(fq_new_copy(x), y, r, p);
}

fq_t fq_self_pow(fq_t x, uint64_t n, zp_poly_t r, uint64_t p)
{
    fq_t z = fq_one();

    while (n)
    {
        if (n & 1)
            z = fq_self_mul(z, x, r, p);

        x = fq_self_mul(x, x, r, p);

        n >>= 1;
    }

    fq_move(&x, z);

    return x;
}

fq_t fq_pow(fq_t x, uint64_t n, zp_poly_t r, uint64_t p)
{
    return fq_self_pow(fq_new_copy(x), n, r, p);
}

fq_t fq_self_inv(fq_t x, zp_poly_t r, uint64_t p)
{
    return fq_self_pow(x, pow64(p, zp_poly_deg(r)) - 2, r, p);
}

fq_t fq_inv(fq_t x, zp_poly_t r, uint64_t p)
{
    return fq_self_inv(fq_new_copy(x), r, p);
}

fq_t fq_self_div(fq_t x, fq_t y, zp_poly_t r, uint64_t p)
{
    fq_t z = fq_inv(y, r, p);

    x = fq_self_mul(x, z, r, p);
    fq_del(z);

    return x;
}

fq_t fq_div(fq_t x, fq_t y, zp_poly_t r, uint64_t p)
{
    return fq_self_div(fq_new_copy(x), y, r, p);
}

fq_t fq_rand(zp_poly_t r, uint64_t p)
{
    return fq_from_zp_poly_view(zp_poly_from_int(prand64(0, zp_poly_to_int(r, p) - 1), p));
}

uint64_t fq_to_int(fq_t x, uint64_t p)
{
    return zp_poly_to_int(x.v, p);
}

fq_t fq_from_int(uint64_t x, uint64_t p)
{
    return fq_from_zp_poly_view(zp_poly_from_int(x, p));
}

fq_t fq_from_str(const char *str)
{
    return fq_from_zp_poly_view(zp_poly_from_str(str));
}

void fq_print(FILE *stream, fq_t x)
{
    zp_poly_print(stream, x.v);
}
