// Tests BigIntFixed.h (BigF<N>, fixed-width unsigned, arithmetic mod 2^(64N)).
// Two independent oracles: __uint128_t, which BigF<2> is bit-for-bit (so the
// documented wraparound of both + and - is checked exactly), and a bit-by-bit
// ripple-carry bitset<320> reference for BigF<5>. Covers carry/borrow chains
// across every limb, subtraction that underflows, unsigned comparison of
// values differing only in the top or only in the bottom limb, and
// get/set/clear/flip at every bit index including limb boundaries.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/various/BigIntFixed.h"

mt19937_64 rng(20260829);
int rnd(int n) { return (int) (rng() % (ull) n); }

// ---------- oracle 1: BigF<2> is exactly __uint128_t ----------
BigF<2> from128(u128 v) {
    BigF<2> a;
    a.d[0] = (ull) v, a.d[1] = (ull) (v >> 64);
    return a;
}
u128 to128(const BigF<2>& a) { return (u128) a.d[1] << 64 | a.d[0]; }

u128 rnd128() {
    switch (rnd(7)) {
        case 0: return 0;
        case 1: return 1;
        case 2: return ~(u128) 0;             // all ones
        case 3: return (u128) rng();          // low limb only
        case 4: return (u128) rng() << 64;    // high limb only
        case 5: return (u128) rng() << 63;    // straddles the boundary
    }
    return (u128) rng() << 64 | rng();
}

void test128() {
    F0R (it, 200000) {
        u128 x = rnd128(), y = rnd128();
        BigF<2> a = from128(x), b = from128(y);
        assert(to128(a + b) == (u128) (x + y));
        assert(to128(a - b) == (u128) (x - y));   // wraps when x < y
        assert((a < b) == (x < y));
        assert((a == b) == (x == y));
        BigF<2> c = a;
        c += b, assert(to128(c) == (u128) (x + y));
        c = a, c -= b, assert(to128(c) == (u128) (x - y));
        int i = rnd(128);
        assert(a.bit(i) == (bool) (x >> i & 1));
        c = a, c.flip(i);
        assert(to128(c) == (x ^ (u128) 1 << i));
        c = a, c.set(i);
        assert(to128(c) == (x | (u128) 1 << i));
        c = a, c.set(i, 0);
        assert(to128(c) == (x & ~((u128) 1 << i)));
    }
    F0R (it, 1000) {   // implicit ull constructor
        ull v = rng();
        assert(to128(BigF<2>(v)) == v);
    }
}

// ---------- oracle 2: ripple-carry bitset for BigF<5> ----------
const int K = 5, W = 64 * K;
using BS = bitset<W>;

BS toBS(const BigF<K>& a) {
    BS r;
    F0R (i, K) F0R (j, 64) if (a.d[i] >> j & 1) r[i * 64 + j] = 1;
    return r;
}
BS addBS(BS a, const BS& b) {
    bool c = 0;
    F0R (i, W) {
        bool x = a[i], y = b[i];
        a[i] = x ^ y ^ c;
        c = (x && y) || (c && (x != y));
    }
    return a;
}
BS subBS(BS a, const BS& b) {
    bool c = 0;
    F0R (i, W) {
        bool x = a[i], y = b[i];
        a[i] = x ^ y ^ c;
        c = (!x && (y || c)) || (x && y && c);
    }
    return a;
}
bool lessBS(const BS& a, const BS& b) {
    ROF (i, 0, W) if (a[i] != b[i]) return b[i];
    return 0;
}

BigF<K> rndBig() {
    BigF<K> a;
    int shape = rnd(4), len = rnd(K + 1);
    F0R (i, K) {
        if (shape == 0) a.d[i] = rng();
        else if (shape == 1) a.d[i] = rng() % 2 ? ~0ULL : 0;
        else if (shape == 2) a.d[i] = i < len ? rng() : 0;
    }
    if (shape == 3) F0R (t, 3) a.set(rnd(W));   // a few sparse bits
    return a;
}

void testBS() {
    BigF<K> a = rndBig();
    BS o = toBS(a);
    F0R (it, 200000) {
        BigF<K> b = rndBig();
        BS ob = toBS(b);
        assert((a < b) == lessBS(o, ob));
        assert((a == b) == (o == ob));
        int i = rnd(W);
        switch (rnd(5)) {
            case 0: a += b, o = addBS(o, ob); break;
            case 1: a -= b, o = subBS(o, ob); break;
            case 2: a.set(i), o[i] = 1; break;
            case 3: a.set(i, 0), o[i] = 0; break;
            default: a.flip(i), o[i] = !o[i];
        }
        assert(toBS(a) == o);
        assert(a.bit(i) == o[i]);
    }
}

// ---------- boundaries, by hand ----------
void testEdges() {
    BigF<K> zero, ones, one(1);
    F0R (i, K) ones.d[i] = ~0ULL;
    assert(ones + one == zero);          // carry out of the top limb is lost
    assert(zero - one == ones);          // borrow out of the top limb is lost
    assert(zero < ones && !(ones < zero) && !(zero < zero));
    assert(zero == zero && !(zero == ones));
    assert(zero - zero == zero && ones - ones == zero);

    // a carry chain running out of limb i into limb i + 1
    F0R (i, K) {
        BigF<K> a;
        F0R (j, i + 1) a.d[j] = ~0ULL;
        BigF<K> c = a + one;
        F0R (j, i + 1) assert(c.d[j] == 0);
        if (i + 1 < K) assert(c.d[i + 1] == 1);
        else assert(c == zero);
        assert(c - one == a);            // and the borrow chain back
    }
    // every bit index, including both ends of every limb
    F0R (i, W) {
        BigF<K> a;
        assert(!a.bit(i));
        a.set(i);
        assert(a.bit(i) && a.d[i / 64] == 1ULL << (i % 64));
        F0R (j, W) assert(a.bit(j) == (i == j));
        a.flip(i);
        assert(a == zero);
        a.set(i, 0);
        assert(a == zero);
    }
    // comparison must look at the top limb first
    BigF<K> hi, lo;
    hi.d[K - 1] = 1, lo.d[0] = ~0ULL;
    assert(lo < hi && !(hi < lo));
    // 64-bit limb boundary: 2^64 - 1 plus 1
    BigF<K> big(~0ULL);
    big += BigF<K>(1);
    assert(!big.bit(63) && big.bit(64) && big.d[0] == 0 && big.d[1] == 1);
    big -= BigF<K>(1);
    assert(big.d[0] == ~0ULL && big.d[1] == 0);
    // widths other than 5
    BigF<1> u(~0ULL);
    assert(u + BigF<1>(1) == BigF<1>(0));
    BigF<9> w;
    w.set(64 * 9 - 1);
    assert(w.bit(64 * 9 - 1) && w.d[8] == 1ULL << 63);
    assert(BigF<9>(0) - w == w);          // 2^575 is its own negation
}

int main() {
    test128();
    testBS();
    testEdges();
    cout << "Tests passed!" << endl;
}
