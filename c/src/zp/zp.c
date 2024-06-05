#include "zp/zp.h"
#include "intrinsics.h"
#include "rand.h"

zp_t zp_new(uint64_t x)
{
    return (zp_t){x};
}

zp_t zp_zero()
{
    return zp_new(0);
}

zp_t zp_one()
{
    return zp_new(1);
}

uint64_t zp_as_int(zp_t x)
{
    return x.v;
}

bool zp_is_zero(zp_t x)
{
    return x.v == 0;
}

zp_t zp_add(zp_t x, zp_t y, uint64_t p)
{
    y.v += x.v;

    if ((y.v >= p) | (y.v < x.v))
        y.v -= p;

    return y;
}

zp_t zp_neg(zp_t x, uint64_t p)
{
    if (p == 0)
        return zp_zero();

    return zp_new(p - x.v);
}

zp_t zp_sub(zp_t x, zp_t y, uint64_t p)
{
    return zp_add(x, zp_neg(y, p), p);
}

zp_t zp_mul(zp_t x, zp_t y, uint64_t p)
{
    uint64_t hi;
    uint64_t lo = _mulx64(x.v, y.v, &hi);

    _divx64(hi, lo, p, &hi);

    return zp_new(hi);
}

zp_t zp_pow(zp_t x, uint64_t y, uint64_t p)
{
    zp_t z = zp_one();

    while (y)
    {
        if (y & 1)
            z = zp_mul(z, x, p);

        x = zp_mul(x, x, p);
        y >>= 1;
    }

    return z;
}

zp_t zp_inv(zp_t x, uint64_t p)
{
    zp_t y = zp_pow(x, p - 2, p);

    if (zp_mul(x, y, p).v == 1)
        return y;

    return zp_zero();
}

zp_t zp_div(zp_t x, zp_t y, uint64_t p)
{
    return zp_mul(x, zp_inv(y, p), p);
}

zp_t zp_rand(uint64_t p)
{
    return zp_new(prand32(0, p - 1));
}
