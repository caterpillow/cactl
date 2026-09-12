// Rope benchmark used for doc/rope.md. Compile from the repo root:
// g++ -O2 -std=c++17 doc/scripts/ropebench.cpp && ./a.out [substring of scenario names to run]
// Every scenario runs in a forked child (peak RSS from wait4, 12 GB address-space cap, 60 s alarm),
// 3 times, median reported; rows sharing a group letter+number must agree on the checksum.
// written by Claude (audit)
#include <ext/rope>
#include "../../stress-tests/utilities/template.h"
#include "../../content/data-structures/Rope.h"
#include <chrono>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
double now_ms() { return chrono::duration<double, milli>(chrono::steady_clock::now().time_since_epoch()).count(); }
mt19937 rng(12345);
int R(int n) { return (int) (rng() % (unsigned) n); }

// element types: make a value from an int, fold a value into a checksum. rope<std::pair<int,int>> does not
// compile on g++ 13 (uninitialized_copy_n is ambiguous between std:: via ADL and __gnu_cxx::), so a plain struct
struct P2 { int a, b; };
template<class T> T mk(int x);
template<> int mk(int x) { return x; }
template<> ll mk(int x) { return (ll) x * 1000003; }
template<> char mk(int x) { return (char) ('a' + x % 26); }
template<> P2 mk(int x) { return {x, x ^ 0x5bd1e995}; }
ll key(int x) { return x; } ll key(ll x) { return x; } ll key(char x) { return x; } ll key(P2 x) { return x.a * 31 + x.b; }
template<class S> ll chk(const S &s) { ll h = 0; for (auto x : s) h = h * 1000003 + key(x); return h; }
template<class T> string tname() { return is_same<T, int>::value ? "int" : is_same<T, ll>::value ? "ll" : is_same<T, char>::value ? "char" : "struct{int,int}"; }

// (2) implicit treap, split/merge by size, kth by walking
template<class T> struct Treap {
    vt<int> l, r, sz; vt<unsigned> p; vt<T> v; int root = 0;
    Treap() { l = r = sz = {0}; p = {0}; v = {T()}; }
    int node(T x) { static mt19937 prng(777); l.pb(0); r.pb(0); sz.pb(1); p.pb(prng()); v.pb(x); return size(v) - 1; } // own rng: keeps the op stream identical across structures
    void upd(int t) { sz[t] = 1 + sz[l[t]] + sz[r[t]]; }
    void split(int t, int k, int &a, int &b) { // first k into a
        if (!t) { a = b = 0; return; }
        if (sz[l[t]] < k) split(r[t], k - sz[l[t]] - 1, r[t], b), a = t; else split(l[t], k, a, l[t]), b = t;
        upd(t);
    }
    int merge(int a, int b) {
        if (!a || !b) return a | b;
        if (p[a] > p[b]) { r[a] = merge(r[a], b); upd(a); return a; }
        l[b] = merge(a, l[b]); upd(b); return b;
    }
    int cnt() { return sz[root]; }
    void insert(int pos, T x) { int a, b; split(root, pos, a, b); root = merge(merge(a, node(x)), b); }
    void erase(int pos, int len) { int a, b, c; split(root, pos, a, b); split(b, len, b, c); root = merge(a, c); }
    T kth(int k) { int t = root; while (true) { if (k < sz[l[t]]) t = l[t]; else if (k == sz[l[t]]) return v[t]; else k -= sz[l[t]] + 1, t = r[t]; } }
    void move(int from, int len, int to) { // cut [from, from+len) and insert it at index `to` of the rest
        int a, b, c, d, e; split(root, from, a, b); split(b, len, b, c); a = merge(a, c); split(a, to, d, e); root = merge(merge(d, b), e); }
    template<class F> void each(F f) { vt<int> st; int t = root; while (t || !st.empty()) { while (t) st.pb(t), t = l[t]; t = st.back(); st.pop_back(); f(v[t]); t = r[t]; } }
    ll sum() { ll h = 0; each([&] (T x) { h = h * 1000003 + key(x); }); return h; }
};

// (3) persistent segment tree, point update, point query
struct PST {
    vt<int> L, Rc, V; vt<int> roots; int n;
    PST(const vi &a, size_t cap) { n = size(a); L.reserve(cap); Rc.reserve(cap); V.reserve(cap); roots.pb(build(a, 0, n)); }
    int build(const vi &a, int lo, int hi) { int id = size(V); L.pb(0); Rc.pb(0); V.pb(0);
        if (hi - lo == 1) V[id] = a[lo]; else { int m = (lo + hi) / 2; L[id] = build(a, lo, m); Rc[id] = build(a, m, hi); } return id; }
    int set(int t, int lo, int hi, int x, int val) { int id = size(V); L.pb(L[t]); Rc.pb(Rc[t]); V.pb(V[t]);
        if (hi - lo == 1) V[id] = val; else { int m = (lo + hi) / 2; if (x < m) L[id] = set(L[t], lo, m, x, val); else Rc[id] = set(Rc[t], m, hi, x, val); } return id; }
    int get(int t, int x) { int lo = 0, hi = n; while (hi - lo > 1) { int m = (lo + hi) / 2; if (x < m) t = L[t], hi = m; else t = Rc[t], lo = m; } return V[t]; }
};

// scenario runner: child process, 3 runs, median per timed column; f returns {times, checksum}
struct Out { vt<double> t; ll c; };
map<string, ll> groups; bool cap_hit = false;
template<class F> void run(string grp, string name, F f) {
    fflush(stdout); int pfd[2]; assert(pipe(pfd) == 0); pid_t pid = fork(); assert(pid >= 0);
    if (pid == 0) {
        close(pfd[0]); rlimit rl{12ull << 30, 12ull << 30}; setrlimit(RLIMIT_AS, &rl); alarm(60);
        rng.seed(12345); vt<vt<double>> ts; ll c = 0; int runs = 0;
        F0R (i, 3) { rng.seed(12345); Out o = f(); if (i) assert(o.c == c); c = o.c; ts.pb(o.t); runs++;
            if (accumulate(all(o.t), 0.0) > 20000) break; } // one run past 20 s: do not repeat
        vt<double> med(size(ts[0])); F0R (j, size(med)) { vt<double> col; for (auto &t : ts) col.pb(t[j]); sort(all(col)); med[j] = col[size(col) / 2]; }
        int k = size(med); assert(write(pfd[1], &runs, 4) == 4); assert(write(pfd[1], &k, 4) == 4);
        assert(write(pfd[1], med.data(), 8 * k) == 8 * k); assert(write(pfd[1], &c, 8) == 8); _exit(0);
    }
    close(pfd[1]); int runs = 0, k = 0; vt<double> med; ll c = 0;
    bool ok = read(pfd[0], &runs, 4) == 4 && read(pfd[0], &k, 4) == 4; if (ok) { med.resize(k); ok = read(pfd[0], med.data(), 8 * k) == 8 * k && read(pfd[0], &c, 8) == 8; }
    close(pfd[0]); int st; rusage ru; wait4(pid, &st, 0, &ru);
    printf("%-4s %-58s", grp.c_str(), name.c_str());
    if (!ok || !WIFEXITED(st)) { printf(" %s\n", WIFSIGNALED(st) && WTERMSIG(st) == SIGALRM ? "killed: > 60 s" : WIFSIGNALED(st) && WTERMSIG(st) == SIGABRT ? "aborted: 12 GB address-space cap" : "failed"); return; }
    for (double t : med) printf(" %9.1f ms", t);
    printf(" %7.1f MB%s\n", ru.ru_maxrss / 1024.0, runs < 3 ? " (1 run)" : "");
    if (grp != "-") { if (groups.count(grp)) { if (groups[grp] != c) printf("     ^^^ CHECKSUM MISMATCH in group %s\n", grp.c_str()); } else groups[grp] = c; }
}
vi randvals(int n) { vi a(n); for (int &x : a) x = R(1000000000); return a; }
template<class T> Rope<T> built(int n) { Rope<T> r; F0R (i, n) r.push_back(mk<T>(i)); return r; }
template<class T> Rope<T> fragmented(int n) { Rope<T> r = built<T>(n); F0R (i, 1000000) r.insert(R(size(r) + 1), mk<T>(i)); F0R (i, 1000000) r.erase(R(size(r)), 1); return r; }

template<class T> void insertsB(string grp, int Q, bool vec) {
    string tn = tname<T>();
    run(grp, "rope<" + tn + ">: " + to_string(Q) + " random insert from empty", [&] { Rope<T> r; double t0 = now_ms(); F0R (i, Q) r.insert(R(size(r) + 1), mk<T>(i)); return Out{{now_ms() - t0}, chk(r)}; });
    run(grp, "treap<" + tn + ">", [&] { Treap<T> t; double t0 = now_ms(); F0R (i, Q) t.insert(R(t.cnt() + 1), mk<T>(i)); return Out{{now_ms() - t0}, t.sum()}; });
    if (vec) run(grp, "vector<" + tn + ">", [&] { vt<T> v; double t0 = now_ms(); F0R (i, Q) v.insert(v.begin() + R(size(v) + 1), mk<T>(i)); return Out{{now_ms() - t0}, chk(v)}; });
}
template<class T> void readsF(string grp, int N, bool frag) {
    string tn = tname<T>(), what = "rope<" + tn + "> " + (frag ? "fragmented" : "built") + " N=" + to_string(N) + ": 1e6 random reads r[i]";
    run(grp, what, [&] { Rope<T> r = frag ? fragmented<T>(N) : built<T>(N); vi q(1000000); for (int &x : q) x = R(N); double t0 = now_ms(); ll h = 0; for (int x : q) h += key(r[x]); return Out{{now_ms() - t0}, h}; });
}
int main(int argc, char **argv) {
    string only = argc > 1 ? argv[1] : "";
    auto want = [&] (const char *s) { return only.empty() || string(s).find(only) != string::npos; };
    run("-", "baseline (empty scenario)", [] { return Out{{0}, 0}; });
    if (want("A")) for (int N : {100000, 1000000}) { string g = "A" + to_string(N / 100000);
        run(g, "rope: " + to_string(N) + " push_back", [&] { double t0 = now_ms(); Rope<int> r; F0R (i, N) r.push_back(i); return Out{{now_ms() - t0}, chk(r)}; });
        run(g, "rope(ptr, N) ctor", [&] { vi a(N); iota(all(a), 0); double t0 = now_ms(); Rope<int> r(a.data(), N); return Out{{now_ms() - t0}, chk(r)}; });
        run(g, "treap: N insert at end", [&] { double t0 = now_ms(); Treap<int> t; F0R (i, N) t.insert(i, i); return Out{{now_ms() - t0}, t.sum()}; });
        run(g, "vector: N push_back", [&] { double t0 = now_ms(); vi v; F0R (i, N) v.pb(i); return Out{{now_ms() - t0}, chk(v)}; });
    }
    if (want("B")) for (int Q : {100000, 300000, 1000000}) insertsB<int>("B" + to_string(Q / 100000), Q, Q <= 300000);
    if (want("C")) for (int Q : {100000, 300000, 1000000}) { string g = "C" + to_string(Q / 100000);
        run(g, "rope: " + to_string(Q) + " random erase(pos, 1) from size Q", [&] { Rope<int> r = built<int>(Q); double t0 = now_ms(); F0R (i, Q) r.erase(R(size(r)), 1); return Out{{now_ms() - t0}, (ll) size(r)}; });
        run(g, "treap", [&] { Treap<int> t; F0R (i, Q) t.insert(i, i); double t0 = now_ms(); F0R (i, Q) t.erase(R(t.cnt()), 1); return Out{{now_ms() - t0}, (ll) t.cnt()}; });
        if (Q <= 300000) run(g, "vector", [&] { vi v(Q); iota(all(v), 0); double t0 = now_ms(); F0R (i, Q) v.erase(v.begin() + R(size(v))); return Out{{now_ms() - t0}, (ll) size(v)}; });
    }
    if (want("D")) { int N = 100000, Q = 1000000;
        run("D", "rope: N=1e5, 1e6 mixed 50/50 insert/erase(pos, 1)", [&] { Rope<int> r = built<int>(N); double t0 = now_ms(); F0R (i, Q) { if (R(2)) r.insert(R(size(r) + 1), i); else r.erase(R(size(r)), 1); } return Out{{now_ms() - t0}, chk(r)}; });
        run("D", "treap", [&] { Treap<int> t; F0R (i, N) t.insert(i, i); double t0 = now_ms(); F0R (i, Q) { if (R(2)) t.insert(R(t.cnt() + 1), i); else t.erase(R(t.cnt()), 1); } return Out{{now_ms() - t0}, t.sum()}; });
        run("D", "vector", [&] { vi v(N); iota(all(v), 0); double t0 = now_ms(); F0R (i, Q) { if (R(2)) v.insert(v.begin() + R(size(v) + 1), i); else v.erase(v.begin() + R(size(v))); } return Out{{now_ms() - t0}, chk(v)}; });
    }
    if (want("E")) for (int N : {100000, 1000000}) for (int big : {0, 1}) for (int front : {0, 1}) { int Q = 100000, ML = big ? N / 10 : 100;
        string g = "E" + to_string(N / 100000) + to_string(big) + to_string(front);
        string desc = "N=" + to_string(N) + ", 1e5 x cut len<=" + to_string(ML) + (front ? " paste at 0" : " paste at random");
        auto gen = [&] (int &l, int &len, int &pos) { len = 1 + R(ML); l = R(N - len + 1); pos = front ? 0 : R(N - len + 1); };
        run(g, "rope: " + desc + " (substr+erase+insert)", [&] { Rope<int> r = built<int>(N); double t0 = now_ms(); F0R (i, Q) { int l, len, pos; gen(l, len, pos); Rope<int> b = r.substr(l, len); r.erase(l, len); r.insert(pos, b); } return Out{{now_ms() - t0}, chk(r)}; });
        run(g, "treap (split x3, merge x3)", [&] { Treap<int> t; F0R (i, N) t.insert(i, i); double t0 = now_ms(); F0R (i, Q) { int l, len, pos; gen(l, len, pos); t.move(l, len, pos); } return Out{{now_ms() - t0}, t.sum()}; });
        run(g, "vector (std::rotate)", [&] { vi v(N); iota(all(v), 0); double t0 = now_ms(); F0R (i, Q) { int l, len, pos; gen(l, len, pos);
            if (pos < l) rotate(v.begin() + pos, v.begin() + l, v.begin() + l + len); else rotate(v.begin() + l, v.begin() + l + len, v.begin() + pos + len); } return Out{{now_ms() - t0}, chk(v)}; });
    }
    if (want("F")) for (int N : {100000, 1000000}) { string g = "F" + to_string(N / 100000);
        readsF<int>(g, N, false);
        run(g, "treap kth", [&] { Treap<int> t; F0R (i, N) t.insert(i, i); vi q(1000000); for (int &x : q) x = R(N); double t0 = now_ms(); ll h = 0; for (int x : q) h += key(t.kth(x)); return Out{{now_ms() - t0}, h}; });
        run(g, "vector v[i]", [&] { vi v(N); iota(all(v), 0); vi q(1000000); for (int &x : q) x = R(N); double t0 = now_ms(); ll h = 0; for (int x : q) h += v[x]; return Out{{now_ms() - t0}, h}; });
        readsF<int>("-", N, true);
    }
    if (want("G")) { int N = 1000000;
        run("G", "rope built N=1e6: range-for sum", [&] { Rope<int> r = built<int>(N); double t0 = now_ms(); ll h = 0; for (int x : r) h += x; return Out{{now_ms() - t0}, h}; });
        run("G", "rope(ptr, N) ctor: range-for sum", [&] { vi a(N); iota(all(a), 0); Rope<int> r(a.data(), N); double t0 = now_ms(); ll h = 0; for (int x : r) h += x; return Out{{now_ms() - t0}, h}; });
        run("-", "rope fragmented N=1e6: range-for sum", [&] { Rope<int> r = fragmented<int>(N); double t0 = now_ms(); ll h = 0; for (int x : r) h += x; return Out{{now_ms() - t0}, h}; });
        run("G", "treap: inorder walk", [&] { Treap<int> t; F0R (i, N) t.insert(i, i); double t0 = now_ms(); ll h = 0; t.each([&] (int x) { h += x; }); return Out{{now_ms() - t0}, h}; });
        run("G", "vector: range-for sum", [&] { vi v(N); iota(all(v), 0); double t0 = now_ms(); ll h = 0; for (int x : v) h += x; return Out{{now_ms() - t0}, h}; });
        run("-", "rope built N=1e5: for i in 0..N-1 sum r[i] (anti-pattern)", [&] { Rope<int> r = built<int>(100000); double t0 = now_ms(); ll h = 0; F0R (i, 100000) h += r[i]; return Out{{now_ms() - t0}, h}; });
        run("-", "rope fragmented N=1e5: same loop over r[i]", [&] { Rope<int> r = fragmented<int>(100000); double t0 = now_ms(); ll h = 0; F0R (i, 100000) h += r[i]; return Out{{now_ms() - t0}, h}; });
    }
    if (want("H")) for (int N : {100000, 1000000}) for (int V : {100000, 1000000}) for (int chain : {0, 1}) {
        string g = "H" + to_string(N / 100000) + to_string(V / 100000) + to_string(chain);
        string desc = "N=" + to_string(N) + " V=" + to_string(V) + (chain ? " chain" : " random parent") + ": [make versions, 1e6 reads]";
        auto gen = [&] (int V, vi &par, vi &x, vi &val, vi &qj, vi &qx) { par.resize(V); x.resize(V); val.resize(V); qj.resize(1000000); qx.resize(1000000);
            F0R (i, V) par[i] = chain ? i : R(i + 1), x[i] = R(N), val[i] = R(1000000000);
            F0R (i, 1000000) qj[i] = R(V + 1), qx[i] = R(N); };
        run(g, "rope " + desc, [&] { vi a = randvals(N), par, x, val, qj, qx; gen(V, par, x, val, qj, qx); vt<Rope<int>> ver; ver.reserve(V + 1); ver.emplace_back(a.data(), N);
            double t0 = now_ms(); F0R (i, V) { ver.pb(ver[par[i]]); ver.back().replace(x[i], val[i]); } double t1 = now_ms();
            ll h = 0; F0R (i, 1000000) h = h * 1000003 + ver[qj[i]][qx[i]]; double t2 = now_ms(); return Out{{t1 - t0, t2 - t1}, h}; });
        run(g, "persistent segtree", [&] { vi a = randvals(N), par, x, val, qj, qx; gen(V, par, x, val, qj, qx); PST st(a, 2 * N + (size_t) V * 22); st.roots.reserve(V + 1);
            double t0 = now_ms(); F0R (i, V) st.roots.pb(st.set(st.roots[par[i]], 0, N, x[i], val[i])); double t1 = now_ms();
            ll h = 0; F0R (i, 1000000) h = h * 1000003 + st.get(st.roots[qj[i]], qx[i]); double t2 = now_ms(); return Out{{t1 - t0, t2 - t1}, h}; });
    }
    if (want("I")) { insertsB<char>("Ic", 300000, true); insertsB<ll>("Il", 300000, true); insertsB<P2>("Ip", 300000, true);
        readsF<char>("-", 1000000, false); readsF<ll>("-", 1000000, false); readsF<P2>("-", 1000000, false);
        for (int t : {0, 1, 2}) run("-", string("rope<") + (t == 0 ? "char" : t == 1 ? "ll" : "struct{int,int}") + "> after 3e5 random inserts: 1e6 random reads", [&] {
            vi q(1000000); for (int &x : q) x = R(300000); ll h = 0; double t0 = 0;
            if (t == 0) { Rope<char> r; F0R (i, 300000) r.insert(R(size(r) + 1), mk<char>(i)); t0 = now_ms(); for (int x : q) h += key(r[x]); }
            if (t == 1) { Rope<ll> r; F0R (i, 300000) r.insert(R(size(r) + 1), mk<ll>(i)); t0 = now_ms(); for (int x : q) h += key(r[x]); }
            if (t == 2) { Rope<P2> r; F0R (i, 300000) r.insert(R(size(r) + 1), mk<P2>(i)); t0 = now_ms(); for (int x : q) h += key(r[x]); }
            return Out{{now_ms() - t0}, h}; });
    }
    if (want("J")) for (int ctor : {0, 1}) run("-", string("rope N=1e6 ") + (ctor ? "ctor" : "push_back") + ": 10 rounds of [1e5 inserts, 1e5 reads]", [&] {
        vi a(1000000); iota(all(a), 0); Rope<int> r = ctor ? Rope<int>(a.data(), 1000000) : built<int>(1000000); vt<double> ts; ll h = 0;
        F0R (round, 10) { double t0 = now_ms(); F0R (i, 100000) r.insert(R(size(r) + 1), i); double t1 = now_ms(); F0R (i, 100000) h += r[R(size(r))]; ts.pb(t1 - t0); ts.pb(now_ms() - t1); }
        return Out{ts, h}; });
}
