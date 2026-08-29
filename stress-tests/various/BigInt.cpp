// Tests BigInt.h (Big, arbitrary-length unsigned, trimmed representation).
// Two independent oracles: __uint128_t for values that fit in 128 bits, and a
// bit-by-bit ripple-carry bitset<512> reference for long random sequences of
// add / subtract / set / clear / flip / compare. Checks the no-leading-zero
// invariant after every operation, comparison of different lengths, growth on
// set and flip past the end, clearing past the end being a no-op, shrinking
// when the top bit goes away, and bit indices at and beyond limb boundaries.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/various/BigInt.h"

mt19937_64 rng(20260829);
int rnd(int n) { return (int) (rng() % (ull) n); }

void inv(const Big& a) { assert(!size(a.d) || a.d.back()); }

// ---------- oracle 1: __uint128_t, for values below 2^127 ----------
Big from128(u128 v) {
    Big a;
    a.d = {(ull) v, (ull) (v >> 64)};
    a.trim();
    return a;
}
u128 to128(const Big& a) {
    u128 v = 0;
    ROF (i, 0, size(a.d)) v = v << 64 | a.d[i];
    return v;
}
u128 rnd127() {
    switch (rnd(7)) {
        case 0: return 0;
        case 1: return 1;
        case 2: return ~(u128) 0 >> 1;         // 2^127 - 1
        case 3: return (u128) rng();           // one limb
        case 4: return (u128) ~0ULL;           // limb of all ones
        case 5: return (u128) rng() << 63;     // straddles the boundary
    }
    return ((u128) rng() << 64 | rng()) >> 1;
}

void test128() {
    F0R (it, 200000) {
        u128 x = rnd127(), y = rnd127();
        if (x < y) swap(x, y);                 // a -= b requires a >= b
        Big a = from128(x), b = from128(y);
        inv(a), inv(b);
        Big p = a + b, q = a - b;
        inv(p), inv(q);
        assert(to128(p) == x + y);
        assert(to128(q) == x - y);
        assert((a < b) == (x < y));
        assert((b < a) == (y < x));
        assert((a == b) == (x == y));
        int i = rnd(130);                      // 2 indices past the top limb
        assert(a.bit(i) == (i < 128 && (bool) (x >> i & 1)));
        assert(to128(Big((ull) x)) == (ull) x);
    }
}

// ---------- oracle 2: ripple-carry bitset ----------
const int W = 512, LIM = 6;                    // reset a above LIM limbs
using BS = bitset<W>;

BS toBS(const Big& a) {
    BS r;
    assert(size(a.d) <= W / 64);
    F0R (i, size(a.d)) F0R (j, 64) if (a.d[i] >> j & 1) r[i * 64 + j] = 1;
    return r;
}
BS addBS(BS a, const BS& b) {
    bool c = 0;
    F0R (i, W) {
        bool x = a[i], y = b[i];
        a[i] = x ^ y ^ c;
        c = (x && y) || (c && (x != y));
    }
    assert(!c);                                // no wraparound expected here
    return a;
}
BS subBS(BS a, const BS& b) {
    bool c = 0;
    F0R (i, W) {
        bool x = a[i], y = b[i];
        a[i] = x ^ y ^ c;
        c = (!x && (y || c)) || (x && y && c);
    }
    assert(!c);
    return a;
}
bool lessBS(const BS& a, const BS& b) {
    ROF (i, 0, W) if (a[i] != b[i]) return b[i];
    return 0;
}

Big rndBig(int len) {
    Big a;
    int shape = rnd(4);
    F0R (i, len) {
        if (shape == 0) a.d.pb(rng());
        else if (shape == 1) a.d.pb(rng() % 2 ? ~0ULL : 0);
        else if (shape == 2) a.d.pb(rng() % 4 ? 0 : rng());
        else a.d.pb(i + 1 == len);             // exactly one high bit
    }
    a.trim();
    inv(a);
    return a;
}

void testBS() {
    Big a = rndBig(rnd(5));
    BS o = toBS(a);
    F0R (it, 200000) {
        Big b = rndBig(rnd(5));
        BS ob = toBS(b);
        assert((a < b) == lessBS(o, ob));
        assert((b < a) == lessBS(ob, o));
        assert((a == b) == (o == ob));
        int i = rnd(W);
        switch (rnd(5)) {
            case 0: a += b, o = addBS(o, ob); break;
            case 1:
                if (a < b) swap(a, b), swap(o, ob);
                a -= b, o = subBS(o, ob);
                break;
            case 2: a.set(i), o[i] = 1; break;
            case 3: a.set(i, 0), o[i] = 0; break;
            default: a.flip(i), o[i] = !o[i];
        }
        inv(a);
        assert(toBS(a) == o);
        assert(a.bit(i) == o[i]);
        F0R (t, 4) {                           // reads past the end are 0
            int j = rnd(W + 200);
            assert(a.bit(j) == (j < W && o[j]));
        }
        if (size(a.d) > LIM) a = rndBig(rnd(5)), o = toBS(a);
    }
}

// ---------- length and bit-index boundaries, by hand ----------
void testEdges() {
    Big zero, one(1);
    assert(size(zero.d) == 0 && size(one.d) == 1);
    assert(zero == Big(0) && zero < one && !(one < zero));
    assert(one - one == zero && zero + zero == zero);
    inv(zero + zero), inv(one - one);
    assert(size((one - one).d) == 0);          // subtraction trims

    // growth on set / flip, and clearing past the end is a no-op
    F0R (i, 400) {
        Big a;
        a.set(i, 0);
        assert(size(a.d) == 0);
        a.set(i);
        assert(size(a.d) == i / 64 + 1 && a.bit(i));
        assert(a.d[i / 64] == 1ULL << (i % 64));
        F0R (j, i / 64) assert(a.d[j] == 0);
        a.set(i, 0);                           // clearing the top bit shrinks
        assert(size(a.d) == 0 && !a.bit(i));
        a.flip(i), a.flip(i);
        assert(size(a.d) == 0);
    }
    // carry chain that adds a limb, and the borrow chain back
    FOR (n, 1, 6) {
        Big a;
        F0R (i, n) a.d.pb(~0ULL);              // 2^(64n) - 1
        Big b = a + one;
        inv(b);
        assert(size(b.d) == n + 1 && b.d[n] == 1);
        F0R (i, n) assert(b.d[i] == 0);
        Big c = b - one;
        inv(c);
        assert(c == a && size(c.d) == n);
    }
    // comparison is by length first, then from the top limb down
    Big lo, hi;
    lo.d = {~0ULL}, hi.d = {0, 1};
    assert(lo < hi && !(hi < lo) && !(lo == hi));
    assert(lo + one == hi && hi - one == lo);
    Big p, q;
    p.d = {0, 5}, q.d = {~0ULL, 5};
    assert(p < q && !(q < p));
    // reading far past the end
    assert(!one.bit(64) && !one.bit(1000) && one.bit(0));
    // a += a and a -= a alias safely
    Big r;
    r.d = {~0ULL, 1};
    Big rr = r;
    r += r;
    assert(r == rr + rr && r.bit(64) && r.bit(0) == 0);
    r -= r;
    assert(r == zero && size(r.d) == 0);
}

int main() {
    test128();
    testBS();
    testEdges();
    cout << "Tests passed!" << endl;
}
