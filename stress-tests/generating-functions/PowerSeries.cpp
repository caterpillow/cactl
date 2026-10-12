// Tests PowerSeries.h: polyInv/polyLog/polyExp/polySqrt vs naive O(n^2)
// recurrences; polyPow vs binary exponentiation with naive truncated
// products (k up to 2^63 - 1, leading zeros, t*k >= n, t*k overflowing 64
// bits, zero series, k = 0, 1, multiples of mod); identities, Fibonacci
// and Catalan numbers up to n = 2^16; output sizes; inputs shorter and
// longer than n.
// written by Claude
#include "../utilities/template.h"

namespace ignore { // NTT.h needs mod = 998244353, not ModPow's 1e9+7
#include "../../content/number-theory/ModPow.h"
}
const ll mod = 998244353;
ll mpow(ll b, ll e = mod - 2) {
    ll ans = 1;
    for (; e; b = b * b % mod, e /= 2) if (e & 1) ans = ans * b % mod;
    return ans;
}
// NTT.h's own `int inv = mpow(..)` warns under -Wconversion (pre-existing);
// silence it for that file only, PowerSeries.h below is checked in full.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wconversion"
#include "../../content/numerical/NumberTheoreticTransform.h"
#pragma GCC diagnostic pop
#include "../../content/generating-functions/PowerSeries.h"

mt19937_64 rng(2026);
ll rnd() { // random residue, biased towards 0, 1, -1 and small values
    switch (rng() % 8) {
        case 0: return 0;
        case 1: return 1;
        case 2: return mod - 1;
        case 3: return ll(rng() % 5);
        default: return ll(rng() % mod);
    }
}
ll rndNZ() { ll x; do x = rnd(); while (!x); return x; }
int rndInt(int lo, int hi) { return lo + int(rng() % unsigned(hi - lo + 1)); }
// random series of random length (sometimes much longer than n) with a
// forced constant term c0 (c0 = -1: any nonzero, -2: anything)
vl gen(int n, ll c0, int minLen = 1) {
    int len = rng() % 4 ? rndInt(minLen, n + 3) : rndInt(minLen, 2 * n + 40);
    vl a(len);
    for (ll& x : a) x = rnd();
    if (len && c0 >= 0) a[0] = c0;
    if (len && c0 == -1) a[0] = rndNZ();
    return a;
}
ll at(const vl& a, int i) { return i < size(a) ? a[i] : 0; }

vl mulN(const vl& a, const vl& b, int n) { // a * b mod x^n
    vl c(n);
    F0R (i, min(n, size(a))) F0R (j, min(n - i, size(b)))
        c[i + j] = (c[i + j] + a[i] * b[j]) % mod;
    return c;
}
vl invN(const vl& a, int n) { // a b = 1: b_i = -a_0^{-1} sum_{j>=1} a_j b_{i-j}
    vl b(n);
    ll i0 = mpow(a[0]);
    F0R (i, n) {
        ll v = !i;
        FOR (j, 1, i + 1) v = (v + (mod - at(a, j)) * b[i - j]) % mod;
        b[i] = v * i0 % mod;
    }
    return b;
}
vl expN(const vl& a, int n) { // y = e^a: i y_i = sum_{j=1}^{i} j a_j y_{i-j}
    vl y(n);
    F0R (i, n) {
        if (!i) { y[i] = 1; continue; }
        ll v = 0;
        FOR (j, 1, i + 1) v = (v + j * at(a, j) % mod * y[i - j]) % mod;
        y[i] = v * mpow(i) % mod;
    }
    return y;
}
vl logN(const vl& a, int n) { // a = e^y: i a_i = sum_{j=1}^{i} j y_j a_{i-j}
    vl y(n);
    FOR (i, 1, n) {
        ll v = at(a, i) * i % mod;
        FOR (j, 1, i) v = (v + mod - j * y[j] % mod * at(a, i - j) % mod) % mod;
        y[i] = v * mpow(i) % mod; // a_0 = 1
    }
    return y;
}
vl sqrtN(const vl& a, int n) { // b^2 = a, b_0 = 1
    vl b(n);
    F0R (i, n) {
        if (!i) { b[i] = 1; continue; }
        ll v = at(a, i);
        FOR (j, 1, i) v = (v + mod - b[j] * b[i - j] % mod) % mod;
        b[i] = v * mpow(2) % mod;
    }
    return b;
}
vl powN(vl a, ll k, int n) { // binary exponentiation, naive products
    vl r(n);
    if (n) r[0] = 1;
    a.resize(n);
    for (; k; k /= 2) {
        if (k & 1) r = mulN(r, a, n);
        if (k > 1) a = mulN(a, a, n);
    }
    return r;
}
vl one(int n) { vl r(n); if (n) r[0] = 1; return r; }

void checkNaive(int n) {
    { // inverse
        vl a = gen(n, -1), b = polyInv(a, n);
        assert(size(b) == n && b == invN(a, n));
        assert(mulN(a, b, n) == one(n));
    }
    { // log
        vl a = gen(n, 1), l = polyLog(a, n);
        assert(size(l) == n && l == logN(a, n));
    }
    { // exp, a may be empty
        vl a = gen(n, 0, 0), e = polyExp(a, n);
        assert(size(e) == n && e == expN(a, n));
    }
    { // sqrt
        vl a = gen(n, 1), q = polySqrt(a, n);
        assert(size(q) == n && q == sqrtN(a, n));
        assert(mulN(q, q, n) == mulN(a, one(n), n));
    }
    { // exp(log a) = a, log(exp a) = a (n = 0: exp a is empty, no a_0)
        vl a = gen(n, 1), b = gen(n, 0, 0);
        assert(polyExp(polyLog(a, n), n) == mulN(a, one(n), n));
        if (n) assert(polyLog(polyExp(b, n), n) == mulN(b, one(n), n));
    }
}

ll rndK() {
    switch (rng() % 6) {
        case 0: return ll(rng() % 4);
        case 1: return ll(rng() % 40);
        case 2: return ll(rng() % ll(1e18)) + 1;
        case 3: return rng() % 2 ? ll(1e18) - ll(rng() % 3)
                                 : LLONG_MAX - ll(rng() % 3);
        case 4: { // near multiples of mod and of mod - 1
            ll m = rng() % 2 ? mod : mod - 1;
            return m * ll(1 + rng() % 3) + ll(rng() % 3) - 1;
        }
        default: return ll(rng() % ll(1e6));
    }
}

void checkPow(int n) {
    // random a with t leading zeros (t = size(a): the zero series)
    int len = rndInt(0, n + 6), t = rng() % 3 ? rndInt(0, 2) : rndInt(0, len);
    t = min(t, len);
    vl a(len);
    FOR (i, t, len) a[i] = i == t ? rndNZ() : rnd();
    vt<ll> ks = {0, 1, 2, rndK(), rndK(), rndK()};
    if (t) for (ll d : {-1, 0, 1}) ks.pb(max(0LL, ll((n - 1) / t) + d));
    if (t) ks.pb(ll(n) / t), ks.pb((ll(n) + t - 1) / t);
    for (ll k : ks) {
        vl r = polyPow(a, k, n);
        assert(size(r) == n && r == powN(a, k, n));
    }
}

int main() {
    vi sizes = {0, 1, 2, 3, 4, 5, 7, 8, 9, 16, 17};
    for (int b = 32; b <= 512; b *= 2) for (int d : {-1, 0, 1}) sizes.pb(b + d);
    sizes.pb(100), sizes.pb(1000);
    for (int n : sizes) F0R (it, n <= 20 ? 40 : n <= 300 ? 6 : 1) checkNaive(n);
    F0R (it, 300) checkNaive(rndInt(0, 70));

    // polyPow vs naive powers, n <= 60
    F0R (n, 61) F0R (it, 12) checkPow(n);
    // all-zero and empty series: 0^0 = 1, 0^k = 0
    for (int n : {0, 1, 2, 5}) for (int len : {0, 1, 3, 9}) {
        vl z(len);
        assert(polyPow(z, 0, n) == one(n));
        for (ll k : {1LL, 2LL, 7LL, ll(1e18), LLONG_MAX}) assert(polyPow(z, k, n) == vl(n));
    }
    // k = 1 is a mod x^n; k = mod with a_0 = 1 gives 1 (Frobenius)
    F0R (it, 50) {
        int n = rndInt(0, 60);
        vl a = gen(n, -2, 0);
        assert(polyPow(a, 1, n) == mulN(a, one(n), n));
        vl b = gen(n, 1);
        assert(polyPow(b, mod, n) == one(n));
    }
    // t * k >= n where t * k overflows 64 bits; k = ceil(2^64 / t) makes
    // t * k wrap to a small value in [0, t) (t = 1, 2: k does not fit)
    FOR (t, 1, 70) {
        unsigned long long w = (~0ULL) / unsigned(t) + 1;
        vl ks = {ll(1e18), LLONG_MAX};
        if (t > 1) ks.pb(LLONG_MAX / t + 1); // t * k > LLONG_MAX
        if (t > 2) ks.pb(ll(w));
        if (t >= 19) assert(w <= 1000000000000000000ULL);
        for (int n : {1, 5, 64, 100}) for (ll k : ks) {
            vl a(t + 3);
            a[t] = rndNZ(), a[t + 1] = rnd();
            assert(polyPow(a, k, n) == vl(n));
        }
    }
    // boundary t * k = n - 1 (single nonzero term c^k at n - 1)
    FOR (t, 1, 12) FOR (n, 1, 40) {
        vl a(t + 2);
        a[t] = rndNZ(), a[t + 1] = rnd();
        ll k = (n - 1) / t;
        vl want = powN(a, k, n);
        assert(polyPow(a, k, n) == want && polyPow(a, k + 1, n) == powN(a, k + 1, n));
    }

    // big n: identities only (n = 2^16 and neighbours)
    for (int n : {(1 << 16) - 1, 1 << 16, (1 << 16) + 1}) {
        vl a = gen(n, -1), b = polyInv(a, n);
        assert(size(b) == n && cut(conv(cut(a, n), b), n) == one(n));
        vl c = gen(n, 1), l = polyLog(c, n);
        assert(size(l) == n && polyExp(l, n) == cut(c, n));
        vl d = gen(n, 0, 0), e = polyExp(d, n);
        assert(size(e) == n && polyLog(e, n) == cut(d, n));
        vl q = polySqrt(c, n);
        assert(size(q) == n && q[0] == 1 && cut(conv(q, q), n) == cut(c, n));
        // pow: k = 3 by products, a^(k1 + k2) = a^k1 a^k2, shifted input
        vl a3 = cut(conv(cut(conv(cut(a, n), cut(a, n)), n), cut(a, n)), n);
        assert(polyPow(a, 3, n) == a3);
        ll k1 = ll(rng() % ll(5e17)), k2 = ll(rng() % ll(5e17));
        vl p1 = polyPow(a, k1, n), p2 = polyPow(a, k2, n);
        assert(polyPow(a, k1 + k2, n) == cut(conv(p1, p2), n));
        int t = rndInt(1, 50), k = rndInt(1, 40);
        vl sh(t, 0);
        sh.insert(sh.end(), a.begin(), a.end());
        vl r = polyPow(sh, k, n), r2 = polyPow(a, k, n - t * k);
        assert(vl(r.begin(), r.begin() + t * k) == vl(t * k, 0));
        assert(vl(r.begin() + t * k, r.end()) == r2);
    }

    // Fibonacci: 1 / (1 - x - x^2) = sum F_{i+1} x^i
    {
        const int N = 1 << 16;
        vl fib = polyInv({1, mod - 1, mod - 1}, N);
        ll x = 0, y = 1; // F_0, F_1
        F0R (i, N) {
            assert(fib[i] == y);
            tie(x, y) = make_pair(y, (x + y) % mod);
        }
    }
    // Catalan: sqrt(1 - 4x) = 1 - 2 sum_{i>=1} C_{i-1} x^i
    {
        const int N = 1 << 16;
        vl fact(2 * N + 1, 1), ifact(2 * N + 1);
        FOR (i, 1, 2 * N + 1) fact[i] = fact[i - 1] * i % mod;
        F0R (i, 2 * N + 1) ifact[i] = mpow(fact[i]);
        vl q = polySqrt({1, mod - 4}, N);
        assert(size(q) == N && q[0] == 1);
        ll inv2 = mpow(2);
        FOR (i, 1, N) {
            int m = i - 1; // C_m = (2m)! / (m! (m+1)!)
            ll cat = fact[2 * m] * ifact[m] % mod * ifact[m + 1] % mod;
            assert((mod - q[i]) * inv2 % mod == cat);
        }
        vl small = polySqrt({1, mod - 4}, 8);
        vl want = {1, mod - 2, mod - 2, mod - 4, mod - 10, mod - 28,
                   mod - 84, mod - 264};
        assert(small == want);
    }
    // exp(x) = sum x^i / i!
    {
        const int N = 1000;
        vl e = polyExp({0, 1}, N);
        ll fct = 1;
        F0R (i, N) {
            if (i) fct = fct * i % mod;
            assert(e[i] * fct % mod == 1);
        }
    }
    cout << "Tests passed!" << endl;
}
