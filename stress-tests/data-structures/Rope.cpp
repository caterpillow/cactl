// Stress test for content/data-structures/Rope.h (__gnu_cxx::rope alias).
// Models a Rope<T> with std::vector<T> for T = int, ll, char and structs
// wrapping pair<int,int> / string (std types themselves do not compile, see
// below): push/pop_back, insert/erase/replace/substr by index and iterator,
// mutable_reference_at, *(mutable_begin()+k) = v, [], at, front/back, +, +=,
// ==, <, swap, clear, range-for / reverse / const iteration, c_str, the
// (ptr, len) ctor, cut-and-paste blocks, O(1) persistent copies (snapshots
// kept alive across many ops, versions[] pattern). Deterministic sweep over
// sizes 0..60 (crosses the 23-element leaf-copy threshold), medium runs
// (~1e3 elements) per type and a long run (1e5 ops, up to 1e5 elements).
// Written by Claude (audit).
#include <ext/rope> // size() members: must be above the template's size macro
#include "../utilities/template.h"
#include "../../content/data-structures/Rope.h"

mt19937 rng(20260913);
int rnd(int a, int b) { return uniform_int_distribution<int>(a, b)(rng); }

template<class T> T gen();
template<> int gen() { return rnd(-1000, 1000); }
template<> ll gen() { return (ll)rnd(-1000, 1000) * 1000000007LL; }
template<> char gen() { return (char)('a' + rnd(0, 25)); }
// Element types from namespace std (pair, string, ...) do not compile:
// ADL makes rope's unqualified uninitialized_copy_n ambiguous with std's.
// So pair and string are wrapped in structs (the string one is non-trivial).
struct PI { pi v; bool operator==(PI o) const { return v == o.v; }
    bool operator<(PI o) const { return v < o.v; } };
struct Str { string v; bool operator==(const Str& o) const { return v == o.v; }
    bool operator<(const Str& o) const { return v < o.v; } };
template<> PI gen() { return {{rnd(0, 9), rnd(0, 9)}}; }
template<> Str gen() { // sometimes longer than SSO
    string t(rnd(0, 1) ? rnd(0, 4) : rnd(16, 24), 'x');
    for (char& c : t) c = gen<char>();
    return {t};
}

template<class T> Rope<T> mk(const vt<T>& v) {
    return v.empty() ? Rope<T>() : Rope<T>(v.data(), (size_t) size(v));
}

template<class T> void check(const Rope<T>& r, const vt<T>& v) {
    assert(size(r) == size(v) && r.empty() == v.empty());
    int i = 0;
    for (const T& x : r) assert(x == v[i++]);        // range-for
    assert(i == size(v));
    for (auto it = r.rbegin(); it != r.rend(); ++it) // reverse
        assert(*it == v[--i]);
    assert(i == 0);
    typename Rope<T>::const_iterator it = r.begin();
    if (!v.empty()) {
        assert(r.front() == v.front() && r.back() == v.back());
        int k = rnd(0, size(v) - 1);
        assert(*(it + k) == v[k] && r[k] == v[k] && r.at(k) == v[k]);
    }
    assert(r == mk(v) && !(r < mk(v)) && !(mk(v) < r));
    if constexpr (is_same<T, char>::value)
        assert(string(r.c_str()) == string(all(v)));
}

template<class T> void spot(const Rope<T>& r, const vt<T>& v) {
    assert(size(r) == size(v));
    if (v.empty()) return;
    int k = rnd(0, size(v) - 1);
    assert(r[k] == v[k] && r.front() == v.front() && r.back() == v.back());
}

// random op mix; cap = max size (big caps: prefilled, ranges bounded so the
// rope stays big); snapshots are kept alive and verified at the end
template<class T> void run(int ops, int cap, int period) {
    Rope<T> r;
    vt<T> v;
    vt<pair<Rope<T>, vt<T>>> snaps;
    bool big = cap > 5000;
    if (big) F0R(i, cap * 9 / 10) v.pb(gen<T>()), r.push_back(v.back());
    int mx = 0;
    F0R(op, ops) {
        int n = size(v), t = rnd(0, 19);
        mx = max(mx, n);
        if (n >= cap && t < 4) t = 4 + rnd(0, 1);
        if (big && n < cap * 4 / 5 && t >= 4 && t <= 7) t = rnd(0, 3);
        int span = big ? 64 : n / 16 + 1; // max range length
        if (t <= 1 || n == 0) {                  // push_back / +=
            T x = gen<T>();
            if (t & 1) r.push_back(x); else r += x;
            v.pb(x);
        } else if (t == 2) {                     // insert(pos, val)
            int p = rnd(0, n); T x = gen<T>();
            r.insert(p, x); v.insert(v.begin() + p, x);
        } else if (t == 3) {                     // insert(iterator, val)
            int p = rnd(0, n); T x = gen<T>();
            r.insert(r.mutable_begin() + p, x); v.insert(v.begin() + p, x);
        } else if (t == 4) {                     // erase(pos, 1) / pop_back
            int p = rnd(0, n - 1);
            if (rnd(0, 3)) r.erase(p, 1); else p = n - 1, r.pop_back();
            v.erase(v.begin() + p);
        } else if (t == 5) {                     // erase(pos, len), ends, all
            int a = rnd(0, n), b = min(n, a + rnd(0, span));
            if (!big && rnd(0, 49) == 0) a = 0, b = n; // whole rope
            if (rnd(0, 5) == 0) a = max(0, n - rnd(0, span)), b = n;
            r.erase(a, b - a); v.erase(v.begin() + a, v.begin() + b);
        } else if (t == 6) {                     // erase(iterator)
            int p = rnd(0, n - 1);
            r.erase(r.mutable_begin() + p); v.erase(v.begin() + p);
        } else if (t == 7) {                     // erase(it1, it2)
            int a = rnd(0, n), b = min(n, a + rnd(0, span));
            r.erase(r.mutable_begin() + a, r.mutable_begin() + b);
            v.erase(v.begin() + a, v.begin() + b);
        } else if (t == 8) {                     // substr
            int a = rnd(0, n), len = rnd(0, min(span, n - a));
            check(r.substr(a, len), vt<T>(v.begin() + a, v.begin() + a + len));
        } else if (t == 9) {                     // replace(pos, val)
            int p = rnd(0, n - 1); T x = gen<T>();
            r.replace(p, x); v[p] = x;
        } else if (t == 10) {                    // mutable_reference_at
            int p = rnd(0, n - 1); T x = gen<T>();
            r.mutable_reference_at(p) = x; v[p] = x;
        } else if (t == 11) {                    // *(mutable_begin()+k) = x
            int p = rnd(0, n - 1); T x = gen<T>();
            *(r.mutable_begin() + p) = x; v[p] = x;
        } else if (t == 12) {                    // replace(pos, n, rope)
            int a = rnd(0, n), len = rnd(0, min(span, n - a));
            vt<T> w(rnd(0, 30)); for (T& x : w) x = gen<T>();
            r.replace(a, len, mk(w));
            v.erase(v.begin() + a, v.begin() + a + len);
            v.insert(v.begin() + a, all(w));
        } else if (t == 13) {                    // insert(pos, rope) / + / +=
            vt<T> w(rnd(0, 30)); for (T& x : w) x = gen<T>();
            Rope<T> s2 = mk(w);
            int p = rnd(0, n), how = rnd(0, 2);
            if (how == 0) r.insert(p, s2), v.insert(v.begin() + p, all(w));
            else if (how == 1) r = r + s2, v.insert(v.end(), all(w));
            else r += s2, v.insert(v.end(), all(w));
        } else if (t == 14) {                    // cut a block, paste elsewhere
            int a = rnd(0, n), len = rnd(0, min(span, n - a));
            Rope<T> blk = r.substr(a, len);
            vt<T> wb(v.begin() + a, v.begin() + a + len);
            r.erase(a, len); v.erase(v.begin() + a, v.begin() + a + len);
            int p = rnd(0, n - len);
            r.insert(p, blk); v.insert(v.begin() + p, all(wb));
        } else if (t == 15) {                    // snapshot (O(1) copy)
            if (size(snaps) < 20) snaps.pb({r, v});
            else snaps[rnd(0, 19)] = {r, v};
        } else if (t == 16 && !snaps.empty()) {  // mutate a snapshot copy
            auto& sn = snaps[rnd(0, size(snaps) - 1)];
            int m = size(sn.s);
            if (m) {
                int p = rnd(0, m - 1); T x = gen<T>();
                if (rnd(0, 1)) sn.f.replace(p, x), sn.s[p] = x;
                else sn.f.erase(p, 1), sn.s.erase(sn.s.begin() + p);
            } else sn.f.push_back(gen<T>()), sn.s.pb(sn.f.back());
            spot(sn.f, sn.s);
        } else if (t == 17 || t == 18) {         // on r, or a window of it if big
            int q = big ? rnd(0, n - 1) : 0, m = big ? min(n - q, 200) : n;
            Rope<T> c = big ? r.substr(q, m) : r;
            vt<T> cv(v.begin() + q, v.begin() + q + m);
            Rope<T> orig = c; vt<T> ov = cv;
            int p = rnd(0, m - 1); T x = gen<T>();
            if (t == 17) {                       // copy-assign, swap, clear
                c.replace(p, x); cv[p] = x;
                assert(orig[p] == ov[p] && c[p] == x);
                Rope<T> d; d = c;
                d.insert(0, x); c.swap(d);
                assert(size(c) == m + 1 && size(d) == m && d == mk(cv));
                c.clear(); assert(c.empty() && size(c) == 0 && c == Rope<T>());
                check(d, cv); check(orig, ov);
            } else {                             // == and < vs model
                c.erase(p, 1); cv.erase(cv.begin() + p);
                assert((c < orig) == (cv < ov) && (orig < c) == (ov < cv));
                assert(!(c == orig));
                c = orig; assert(c == orig && !(c < orig));
            }
        } else {                                 // reverse-copy round trip
            if (n < 300) {
                vt<T> w(r.rbegin(), r.rend()); reverse(all(w)); assert(w == v);
            }
        }
        spot(r, v);
        if (op % period == 0) check(r, v);
    }
    check(r, v);
    for (auto& sn : snaps) check(sn.f, sn.s);
    assert(mx >= cap / 2);
}

void sweep() { // sizes 0..60: every single erase, every substr, every insert
    F0R(n, 61) {
        vt<int> v(n); iota(all(v), 1);
        Rope<int> r = mk(v);
        check(r, v);
        F0R(i, n) {
            Rope<int> c = r; vt<int> cv = v;
            c.erase(i, 1); cv.erase(cv.begin() + i);
            check(c, cv); check(r, v);
            c = r; cv = v; c.insert(i, -i - 1); cv.insert(cv.begin() + i, -i - 1);
            check(c, cv);
            c = r; cv = v; c.mutable_reference_at(i) = 7; cv[i] = 7;
            check(c, cv); check(r, v);
        }
        for (int a = 0; a <= n; a += 1 + n / 8)
            for (int b = a; b <= n; b += 1 + n / 8) {
                check(r.substr(a, b - a), vt<int>(v.begin() + a, v.begin() + b));
                Rope<int> c = r; vt<int> cv = v;
                c.erase(a, b - a); cv.erase(cv.begin() + a, cv.begin() + b);
                check(c, cv);
                Rope<int> d = r; d.insert(b, c); vt<int> dv = v;
                dv.insert(dv.begin() + b, all(cv));
                check(d, dv); check(r, v);
            }
    }
}

void versions_pattern() { // persistent array: versions[i] = versions[i-1] + 1 write
    const int N = 1000, V = 2000;
    vt<int> base(N); for (int& x : base) x = gen<int>();
    vt<Rope<int>> ver = {mk(base)};
    vt<pi> upd;
    FOR(i, 1, V) {
        int x = rnd(0, N - 1), val = gen<int>();
        ver.pb(ver[i - 1]);
        ver[i].replace(x, val);
        upd.pb({x, val});
    }
    vt<int> cur = base;
    F0R(i, V) {
        if (i) cur[upd[i - 1].f] = upd[i - 1].s;
        int k = 0;
        for (int x : ver[i]) assert(x == cur[k++]);
        assert(k == N);
    }
    // old version is independent of later ones
    ver[0].erase(0, N / 2);
    assert(size(ver[0]) == N / 2 && ver[0][0] == base[N / 2]);
    vt<int> v1 = base; v1[upd[0].f] = upd[0].s;
    check(ver[1], v1);
}

void ll_literals() {
    Rope<ll> r;
    r.push_back(0LL); r.insert(0, 0LL); r.replace(1, 0LL); r += 0LL;
    r.insert(1, 5LL);
    vt<ll> v = {0, 5, 0, 0};
    check(r, v);
}

void old_erase_bug() {
#if __GNUC__ < 11
    // GCC 7..11.2: erase(p) was erase(p, p + 1), i.e. erased p+1 elements.
    // Demonstrated here (never rely on it): 11.3+/12+ removed the overload.
    Rope<int> r; F0R(i, 10) r.push_back(i);
    r.erase(3);
    assert(size(r) == 10 - 4); // 4 elements gone, not 1
#endif
    Rope<int> r; F0R(i, 10) r.push_back(i);
    r.erase(3, 1); assert(size(r) == 9 && r[3] == 4);
    r.erase(r.mutable_begin() + 3); assert(size(r) == 8 && r[3] == 5);
}

int main() {
    example(); cout << endl;
    ll_literals();
    old_erase_bug();
    sweep();
    versions_pattern();
    run<int>(20000, 1000, 100);
    run<ll>(20000, 1000, 100);
    run<char>(20000, 1000, 100);
    run<PI>(20000, 1000, 100);
    run<Str>(20000, 1000, 100);
    run<int>(100000, 100000, 2000);
    cout << "Tests passed!" << endl;
}
