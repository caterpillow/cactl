// Tests PowerSeriesSlow.h. mod is a runtime variable here (namespace trick),
// so one binary covers 1e9+7, 998244353, 2^31-1 and the small primes 2..101,
// where n <= mod is tight, k is reduced mod p and inv runs with n > mod.
// References are independent of the header: naive O(n^2) products, repeated
// multiplication and binary exponentiation, the geometric series for 1/a,
// the defining series exp(a) = sum a^m/m! and
// ln(1+u) = sum (-1)^(m+1) u^m/m, the ODEs e' = a'e, a l' = a', a b' = k a'b
// checked with naive products (with b's leading terms they determine the
// answer when n <= mod), a trinomial DP, and closed forms (1/i!, 1/i,
// Fibonacci, Bell, central trinomial, Catalan from sqrt(1-4x)). The helper
// ode is also checked against its printed formula at the edges of its stated
// ranges (q, K in [0, mod) incl. K = mod - 1, L in {0, 1}, z = y or not).
// Sizes 0..3, 10, 100, 1000 and around powers of two; inputs shorter and
// longer than n, all-(mod-1) inputs, leading zeros with tk < n, = n, > n,
// a = 0 with k = 0 and k > 0, and k up to 1e18.
// written by Claude
#include "../utilities/template.h"

namespace unused { // ModPow.h's const mod; the header sees ::mod below
#include "../../content/number-theory/ModPow.h"
}
ll mod = 1000000007;
ll mpow(ll b, ll e = mod - 2) {
    ll ans = 1;
    for (; e; b = b * b % mod, e /= 2)
        if (e & 1) ans = ans * b % mod;
    return ans;
}
#include "../../content/generating-functions/PowerSeriesSlow.h"

mt19937_64 rng(2026);
ll rnd() { return ll(rng() % (unsigned long long)mod); }
int rr(int lo, int hi) { // uniform in [lo, hi]
    return lo + int(rng() % (unsigned long long)(hi - lo + 1));
}
vl rvec(int len, bool maxed = false) {
    vl v(len);
    for (ll& x : v) x = maxed ? mod - 1 : rnd();
    return v;
}
vl fit(vl a, int n) { a.resize(n); return a; }
vl one(int n) { vl r(n); if (n) r[0] = 1; return r; }
vl scale(vl a, ll c) { for (ll& x : a) x = x * c % mod; return a; }

vl mul(const vl& a, const vl& b, int n) { // a * b mod x^n, naive
    vl c(n);
    F0R (i, min(size(a), n)) if (a[i]) F0R (j, min(size(b), n - i))
        c[i + j] = (c[i + j] + a[i] * b[j]) % mod;
    return c;
}
vl powNaive(vl a, ll k, int n) { // binary exponentiation, naive products
    vl r = one(n);
    a = fit(a, n);
    for (; k; k >>= 1) {
        if (k & 1) r = mul(r, a, n);
        if (k > 1) a = mul(a, a, n);
    }
    return r;
}
vl deriv(const vl& a) { // a', |a| - 1 terms
    vl d;
    FOR (i, 1, size(a)) d.pb(a[i] * (i % mod) % mod);
    return d;
}
vl invRef(const vl& a, int n) { // q sum_{m < n} u^m, u = 1 - q a, q = 1/a_0
    ll q = mpow(a[0]);
    vl u = scale(fit(a, n), mod - q), r(n), pw = one(n);
    if (n) u[0] = 0;
    F0R (m, n) {
        F0R (i, n) r[i] = (r[i] + pw[i] * q) % mod;
        pw = mul(pw, u, n);
    }
    return r;
}
vl expRef(const vl& a, int n) { // sum_{m < n} a^m / m!, a[0] = 0
    vl r = one(n), term = one(n), b = fit(a, n);
    FOR (m, 1, n) {
        term = scale(mul(term, b, n), mpow(m));
        F0R (i, n) r[i] = (r[i] + term[i]) % mod;
    }
    return r;
}
vl logRef(const vl& a, int n) { // ln(1 + u) = sum (-1)^(m+1) u^m / m
    vl u = fit(a, n), r(n), pw = one(n);
    if (n) u[0] = 0;
    FOR (m, 1, n) {
        pw = mul(pw, u, n);
        ll c = m % 2 ? mpow(m) : (mod - mpow(m)) % mod;
        F0R (i, n) r[i] = (r[i] + pw[i] * c) % mod;
    }
    return r;
}
bool valid(const vl& v, int n) {
    if (size(v) != n) return false;
    for (ll x : v) if (x < 0 || x >= mod) return false;
    return true;
}
vi lens(int n) { // lengths of the inputs: short, about n, longer than n
    if (n > 300) return {2, 4, n + 3};
    return {1, 2, 3, 5, n / 2 + 1, max(n, 1), n + 1, n + 7};
}

void testInv(int n) { // any n: no division by i
    for (int len : lens(n)) for (bool mx : {false, true}) {
        vl a = rvec(len, mx);
        while (!a[0]) a[0] = rnd();
        vl b = invSlow(a, n);
        assert(valid(b, n));
        assert(mul(a, b, n) == one(n));
        if (n <= 40) assert(b == invRef(a, n));
    }
    ll c = 1 + rnd() % (mod - 1);
    vl want(n);
    if (n) want[0] = mpow(c);
    assert(invSlow({c}, n) == want);
}

void testExp(int n) { // n <= mod
    vi ls = lens(n);
    ls.pb(0);
    for (int len : ls) for (bool mx : {false, true}) {
        vl a = rvec(len, mx);
        if (len) a[0] = 0;
        vl e = expSlow(a, n);
        assert(valid(e, n));
        if (!n) continue;
        assert(e[0] == 1);
        assert(deriv(e) == mul(deriv(fit(a, n)), e, n - 1)); // e' = a'e
        if (n <= 40) assert(e == expRef(a, n));
        assert(logSlow(e, n) == fit(a, n));
    }
}

void testLog(int n) { // n <= mod
    for (int len : lens(n)) for (bool mx : {false, true}) {
        vl a = rvec(len, mx);
        a[0] = 1;
        vl l = logSlow(a, n);
        assert(valid(l, n));
        if (!n) continue;
        assert(l[0] == 0);
        vl A = fit(a, n);
        assert(mul(A, deriv(l), n - 1) == deriv(A)); // a l' = a'
        if (n <= 40) assert(l == logRef(a, n));
        assert(expSlow(l, n) == A);
    }
}

// ode vs its printed contract: y_i = q/i sum_{j=1}^{i} (Kj - Li) a_j z_{i-j}
// for 0 < i < |y|, y_0 untouched; z = y means z_{i-j} are the new y's.
void testOde(int n) { // n <= mod
    for (int len : lens(n)) for (ll L : {0, 1})
    for (bool alias : {false, true}) {
        vl a = rvec(len), z = rvec(n + rr(0, 2)), y = rvec(n);
        ll q = rnd(), K = vl{0, 1, mod - 1, rnd()}[rr(0, 3)];
        vl want = y;
        FOR (i, 1, n) {
            __int128 sum = 0;
            for (int j = 1; j <= i && j < len; j++) {
                ll c = ll((((__int128)K * j - L * i) % mod + mod) % mod);
                ll zz = alias ? want[i - j] : z[i - j];
                sum += (__int128)c * a[j] % mod * zz;
            }
            want[i] = ll(sum % mod * q % mod * mpow(i) % mod);
        }
        if (alias) ode(a, y, y, q, K, L);
        else ode(a, y, z, q, K, L);
        assert(valid(y, n) && y == want);
    }
}

// b == a^k mod x^n: zeros below t*k, b[tk] = a_t^k, and a b' = k a' b.
// [x^(m-1)] of it is sum_j (m - j - kj) a_j b_(m-j), whose j = t term fixes
// b_(m-t) (factor m - t - kt != 0 mod p): checked up to m = n - 1 + t, so
// every b_i, i < n, is pinned down when n <= mod.
void checkPow(const vl& a, ll k, int n, const vl& b) {
    assert(valid(b, n));
    int t = 0;
    while (t < size(a) && !a[t]) t++;
    if (!k) { assert(b == one(n)); return; }
    if (t == size(a) || (__int128)t * k >= n) { assert(b == vl(n)); return; }
    int o = int(t * k);
    F0R (i, o) assert(b[i] == 0);
    assert(b[o] == mpow(a[t], k));
    if (n < 2) return;
    vl A = fit(a, n + t), lhs = mul(A, deriv(b), n - 1 + t);
    vl rhs = scale(mul(deriv(A), b, n - 1 + t), k % mod);
    assert(lhs == rhs);
}

void testPow(int n, int iters, bool naive) { // n <= mod
    F0R (it, iters) {
        int t = rr(0, 3), len = n > 300 ? rr(1, 5) : rr(1, n + 3);
        vl a(t), tail = rvec(len, it == 0);
        while (!tail[0]) tail[0] = rnd();
        a.insert(a.end(), all(tail));
        vl ks = {0, 1, 2, 3, rr(4, 20), mod - 1, mod, mod + 1,
                 2 * mod + 3, ll(rng() >> 4), ll(1e18)};
        if (t) for (int d : {-1, 0, 1}) ks.pb(max(0, (n - 1) / t + d));
        for (ll k : ks) {
            vl b = powSlow(a, k, n);
            checkPow(a, k, n, b);
            if (naive) assert(b == powNaive(a, k, n));
            if (naive && k <= 20) { // repeated multiplication
                vl rep = one(n);
                F0R (j, k) rep = mul(rep, a, n);
                assert(b == rep);
            }
        }
    }
}

void testPowEdges(int maxn) { // a = 0, pure monomials, huge shifts
    for (int n = 0; n <= maxn; n++) {
        for (const vl& a : {vl{}, vl{0}, vl{0, 0, 0}, vl(n + 2)}) {
            assert(powSlow(a, 0, n) == one(n));
            for (ll k : {1LL, 2LL, 5LL, ll(1e18)})
                assert(powSlow(a, k, n) == vl(n));
        }
        ll c = 1 + rnd() % (mod - 1);
        for (ll k : {0LL, 1LL, 2LL, ll(n) - 1, ll(n), ll(n) + 1, ll(1e18)}) {
            if (k < 0) continue;
            vl want(n);
            if (k < n) want[k] = mpow(c, k);
            assert(powSlow({0, c}, k, n) == want);
            assert(powSlow({0, c, 0, 0}, k, n) == want);
            vl w2(n); // (c x^2)^k
            if (2 * k < n) w2[2 * k] = mpow(c, k);
            assert(powSlow({0, 0, c}, k, n) == w2);
        }
        vl lead(n + 2); lead.pb(c); // t > n
        assert(powSlow(lead, 0, n) == one(n));
        assert(powSlow(lead, 1, n) == vl(n));
        for (int t : {1, 3, 10, 28, 46}) { // t*k overflows ll for some
            vl a(t);
            a.pb(c), a.pb(1);
            for (ll k : {ll(1e18), ll(1e18) - 1, ll(1e18) / 3})
                assert(powSlow(a, k, n) == vl(n));
        }
        vl a = rvec(rr(1, 6));
        a[0] = 1 + rnd() % (mod - 1);
        assert(powSlow(a, 1, n) == fit(a, n));
        assert(powSlow(a, 2, n) == mul(a, a, n));
    }
}

void testResidue(int n) { // a[0] = 1: k only matters mod p; n <= mod
    vl a = rvec(rr(1, n + 3));
    a[0] = 1;
    ll k = ll(rng() >> 2);
    assert(powSlow(a, k, n) == powSlow(a, k % mod, n));
    assert(powSlow(a, mod - 1, n) == invSlow(a, n)); // k = -1
    if (mod % 2) { // k = 1/2: the square root with r[0] = 1
        vl r = powSlow(a, (mod + 1) / 2, n);
        if (n) assert(r[0] == 1);
        assert(mul(r, r, n) == fit(a, n));
    }
    if (mod > 3) { // k = 1/3
        vl c = powSlow(a, mpow(3), n);
        assert(mul(mul(c, c, n), c, n) == fit(a, n));
    }
}

void testTrinomial(int n, int K) { // (1+x+x^2)^k vs DP, k < K
    vl dp = one(n);
    F0R (k, K) {
        assert(powSlow({1, 1, 1}, k, n) == dp);
        vl nx(n);
        F0R (i, n) nx[i] = (dp[i] + (i ? dp[i - 1] : 0)
                            + (i > 1 ? dp[i - 2] : 0)) % mod;
        dp = nx;
    }
}

void closedForms() { // under a big prime
    const int N = 12;
    vl fact(N, 1);
    FOR (i, 1, N) fact[i] = fact[i - 1] * i % mod;
    vl e = expSlow({0, 1}, N), x1(N, 1); // e^x
    F0R (i, N) assert(e[i] * fact[i] % mod == 1);
    vl l = logSlow(x1, N); // ln 1/(1-x) = sum x^i / i
    FOR (i, 1, N) assert(l[i] * i % mod == 1);
    vl lg = logSlow({1, 1}, N); // ln(1+x)
    FOR (i, 1, N) assert(lg[i] * i % mod == (i % 2 ? 1 : mod - 1));
    vl fib = invSlow({1, mod - 1, mod - 1}, N);
    assert(fib == vl({1, 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144}));
    vl ex(N); // Bell numbers: exp(e^x - 1)
    FOR (i, 1, N) ex[i] = mpow(fact[i]);
    vl bell = expSlow(ex, N);
    F0R (i, N) bell[i] = bell[i] * fact[i] % mod;
    assert(bell == vl({1, 1, 2, 5, 15, 52, 203, 877, 4140, 21147,
                       115975, 678570}));
    F0R (k, N) { // central trinomial coefficients
        static const ll ct[] = {1, 1, 3, 7, 19, 51, 141, 393, 1107, 3139,
                                8953, 25653};
        assert(powSlow({1, 1, 1}, k, k + 1)[k] == ct[k]);
    }
    vl sq = powSlow({1, mod - 4}, (mod + 1) / 2, N + 1); // sqrt(1-4x)
    vl cat = {1, 1, 2, 5, 14, 42, 132, 429, 1430, 4862, 16796, 58786};
    F0R (i, N) assert((mod - sq[i + 1]) * mpow(2) % mod == cat[i]);
    vl r = powSlow({1, 1, mpow(4)}, (mod + 1) / 2, 5); // (2+x)^2 / 4
    assert(scale(r, 2) == vl({2, 1, 0, 0, 0}));
}

int main() {
    vi sizes = {0, 1, 2, 3, 4, 5, 7, 8, 9, 10, 15, 16, 17, 31, 32, 33, 63, 64,
                65, 100, 127, 128, 129, 255, 256, 257, 511, 512, 513, 1000,
                1023, 1024, 1025};
    for (ll p : {1000000007LL, 998244353LL, 2147483647LL}) {
        mod = p;
        closedForms();
        for (int n : sizes) {
            testInv(n), testExp(n), testLog(n), testResidue(n);
            testPow(n, n <= 130 ? 6 : 1, n <= 130);
            testOde(n);
        }
        testPowEdges(12);
        testTrinomial(140, 70);
        vl a = {mod - 1, mod - 1, mod - 1, mod - 1}; // dense, k = 1e18
        int n = p == 1000000007 ? 1000 : 300;
        assert(powSlow(a, ll(1e18), n) == powNaive(a, ll(1e18), n));
    }
    for (ll p : {2, 3, 5, 7, 13, 101}) {
        mod = p;
        for (int n = 0; n <= p; n++) {
            testExp(n), testLog(n), testResidue(n), testOde(n);
            testPow(n, 8, true);
        }
        for (int n : {0, 1, 2, 3, 10, 100, 1000}) testInv(n); // n > mod ok
        testPowEdges(int(p));
        testTrinomial(int(p), 3 * int(p) + 5);
    }
    cout << "Tests passed!" << endl;
}
