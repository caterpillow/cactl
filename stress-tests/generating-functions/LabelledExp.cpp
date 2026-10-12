// Tests LabelledExp.h. labelledExp: vs a brute-force sum over all set
// partitions (i <= 9), Bell numbers (Bell triangle), (k-1)! -> i!,
// involutions (t_i = t_{i-1} + (i-1) t_{i-2}). labelledLog: connected
// graphs vs brute-force enumeration (i <= 7) and A001187 (i <= 11).
// Both: log(exp(a)) == a, exp(log(g)) == g, EGF exp/log through
// factorials (prime mod, every size: 0..40, 63..65, 127..129,
// 255..257, 500, 511..513, random < 700), inputs shorter/longer than n,
// a[0] ignored. Prime, composite, tiny (n >= mod) and ~3e9 moduli.
// mod is a runtime variable here so one binary covers every modulus.
// written by Claude
#include "../utilities/template.h"

namespace unused { // ModPow.h's const mod; the header sees ::mod below
#include "../../content/number-theory/ModPow.h"
}
ll mod = 1000000007;
#include "../../content/generating-functions/LabelledExp.h"

mt19937_64 rng(11);
vl rnd(int n) {
    vl v(n);
    for (ll& x : v) x = ll(rng() % (unsigned long long)mod);
    return v;
}
ll pw(ll b, ll e) {
    ll r = 1 % mod;
    for (b %= mod; e; b = b * b % mod, e /= 2) if (e & 1) r = r * b % mod;
    return r;
}
vl prefix(const vl& v, int n) { return vl(v.begin(), v.begin() + n); }

// sum over all set partitions of {1..i} of prod a[block size]
ll bruteExp(const vl& a, int i) {
    vi blocks; // block sizes; element e joins a block or opens one
    ll tot = 0;
    function<void(int)> rec = [&](int e) {
        if (e == i) {
            ll prod = 1 % mod;
            for (int c : blocks) prod = prod * a[c] % mod;
            tot = (tot + prod) % mod;
            return;
        }
        F0R (j, size(blocks)) blocks[j]++, rec(e + 1), blocks[j]--;
        blocks.pb(1), rec(e + 1), blocks.pop_back();
    };
    rec(0);
    return tot;
}

ll connectedBrute(int n) { // exact, enumerates all 2^C(n,2) graphs
    vt<pi> E;
    F0R (u, n) FOR (v, u + 1, n) E.pb({u, v});
    ll cnt = 0;
    F0R (msk, 1 << size(E)) {
        vi adj(n);
        F0R (e, size(E)) if (msk >> e & 1)
            adj[E[e].first] |= 1 << E[e].second,
            adj[E[e].second] |= 1 << E[e].first;
        int seen = 1, front = 1;
        while (front) {
            int nxt = 0;
            F0R (u, n) if (front >> u & 1) nxt |= adj[u];
            front = nxt & ~seen, seen |= nxt;
        }
        cnt += seen == (1 << n) - 1;
    }
    return cnt;
}

vl bellTriangle(int n) { // Aitken's array, independent of binomials
    vl res, row = {1 % mod};
    F0R (i, n) {
        res.pb(row[0]);
        vl nxt = {row.back()};
        for (ll x : row) nxt.pb((nxt.back() + x) % mod);
        row = nxt;
    }
    return res;
}

// EGF route (prime mod > n): B = exp(A) via B' = A'B, L = log G via
// G' = L'G, on coefficients a_k / k!, then multiply back by i!
vl factorials(int n) {
    vl fa(n + 1, 1 % mod);
    FOR (i, 1, n + 1) fa[i] = fa[i - 1] * i % mod;
    return fa;
}
vl egfExp(const vl& a, int n) {
    vl fa = factorials(n), A(n), B(n);
    FOR (k, 1, min(n, size(a))) A[k] = a[k] * pw(fa[k], mod - 2) % mod;
    F0R (i, n) {
        ll v = !i;
        FOR (k, 1, i + 1) v = (v + k * A[k] % mod * B[i - k]) % mod;
        B[i] = i ? v * pw(i, mod - 2) % mod : v;
    }
    F0R (i, n) B[i] = B[i] * fa[i] % mod;
    return B;
}
vl egfLog(const vl& g, int n) { // g[0] == 1
    vl fa = factorials(n), G(n), L(n);
    F0R (k, min(n, size(g))) G[k] = g[k] * pw(fa[k], mod - 2) % mod;
    FOR (i, 1, n) {
        ll v = 0;
        FOR (k, 1, i) v = (v + k * L[k] % mod * G[i - k]) % mod;
        L[i] = (G[i] - v * pw(i, mod - 2) % mod + mod) % mod;
    }
    F0R (i, n) L[i] = L[i] * fa[i] % mod;
    return L;
}

int main() {
    // edge cases and exact small values, mod = 1e9+7
    assert(labelledExp({}, 0).empty() && labelledLog({1}, 0).empty());
    assert(labelledExp({}, 1) == vl{1} && labelledLog({1}, 1) == vl{0});
    assert(labelledExp(vl({5, 7}), 1) == vl{1}); // a[0] ignored
    assert(labelledExp(vl({5, 7}), 3) == vl({1, 7, 49}));
    assert(labelledExp({}, 4) == vl({1, 0, 0, 0}));
    assert(labelledLog({1}, 4) == vl({0, 0, 0, 0}));
    assert(labelledLog(vl(4, 1), 4) == vl({0, 1, 0, 0})); // log e^x
    // log(1 + x): c_i = (-1)^(i+1) (i-1)!
    assert(labelledLog(vl({1, 1}), 5) == vl({0, 1, mod - 1, 2, mod - 6}));
    vl bell = {1, 1, 2, 5, 15, 52, 203, 877, 4140, 21147, 115975,
               678570, 4213597, 27644437, 190899322};
    assert(labelledExp(vl(15, 1), 15) == bell);
    vl inv = {1, 1, 2, 4, 10, 26, 76, 232, 764, 2620, 9496, 35696};
    assert(labelledExp(vl({0, 1, 1}), 12) == inv);
    vl conn = {0, 1, 1, 4, 38, 728, 26704, 1866256, 251548592,
               66296291072LL, 34496488594816LL, 35641657548953344LL};
    F0R (i, 8) assert(connectedBrute(i) == conn[i]);

    vl mods = {1000000007, 998244353, 2147483647, 2999999929, // primes
               1000000000, 1LL << 31, 3000000000LL, 2, 3, 4, 6, 7, 12,
               97, 1001};
    for (ll m : mods) {
        mod = m;
        bool prime = m > 100000000 && m % 2; // the four primes
        // brute force over set partitions, random and extreme a
        F0R (it, 6) {
            vl a = rnd(10);
            if (it == 0) a.assign(10, mod - 1);
            vl b = labelledExp(a, 10);
            F0R (i, 10) assert(b[i] == bruteExp(a, i));
            a[0] = 0;
            assert(labelledLog(b, 10) == a);
        }
        // Bell numbers, i!, involutions, n >= mod for small moduli
        const int N = 1200;
        vl ones(N + 5, 1), fac(N), cyc(N), t(N);
        vl want = bellTriangle(N);
        assert(labelledExp(ones, N) == want);
        F0R (i, N) fac[i] = i ? fac[i - 1] * i % mod : 1 % mod;
        FOR (k, 1, N) cyc[k] = fac[k - 1];
        assert(labelledExp(cyc, N) == fac);
        F0R (i, N) t[i] = i < 2 ? 1 % mod : (t[i - 1] + (i - 1) * t[i - 2]) % mod;
        assert(labelledExp(vl({0, 1, 1}), N) == t);
        // connected graphs: g_i = 2^C(i,2)
        vl g(12);
        F0R (i, 12) g[i] = pw(2, i * (i - 1) / 2);
        vl c = labelledLog(g, 12);
        F0R (i, 12) assert(c[i] == conn[i] % mod);
        assert(labelledExp(c, 12) == g);
        // round trips and truncation at many sizes
        vi sizes;
        F0R (n, 41) sizes.pb(n);
        for (int b = 64; b <= 512; b *= 2) for (int d : {-1, 0, 1}) sizes.pb(b + d);
        sizes.pb(500), sizes.pb(int(rng() % 700));
        for (int n : sizes) {
            vl a = rnd(n + 3), b = labelledExp(a, n + 3);
            vl got = labelledExp(a, n);
            assert(size(got) == n && got == prefix(b, n));
            vl a2 = prefix(a, n);
            if (n) a2[0] = 0;
            assert(labelledLog(got, n) == a2);
            assert(labelledLog(b, n) == a2); // input longer than n
            vl h = rnd(n);
            if (n) h[0] = 1 % mod;
            vl lh = labelledLog(h, n);
            assert(size(lh) == n && labelledExp(lh, n) == h);
            if (prime) { // every size here is < 700 < mod
                assert(got == egfExp(a, n));
                assert(lh == egfLog(h, n));
            }
        }
    }
    cout << "Tests passed!" << endl;
}
