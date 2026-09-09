// Flow benchmark used for doc/dinic.md. Compile from the repo root, e.g.
// g++ -O2 -std=c++17 -DFLOW_HEADER=\"content/graph/Dinic2.h\" -DINC -DLIM doc/scripts/flowbench.cpp
// (-DFLOW_T=PushRelabel<ll> for PushRelabel.h; INC/LIM need Dinic2.h's adj and calc(s, t, lim)).
// written by Claude (audit)
#include "../../stress-tests/utilities/template.h"
#include FLOW_HEADER
#include <chrono>
#ifndef FLOW_T
#define FLOW_T Dinic
#endif
struct Case { const char *name; int n, src, snk; vt<array<ll, 3>> ed; bool bidir; };
vt<Case> cases() {
    mt19937 rng(42); auto R = [&] (ll lo, ll hi) { return lo + (ll)(rng() % (unsigned long long)(hi - lo + 1)); };
    vt<Case> cs;
    { Case c{"random sparse 1e5/4e5", 100000, 0, 99999, {}, false}; F0R (i, 400000) c.ed.pb({R(0, 99999), R(0, 99999), R(1, 1000000000)}); cs.pb(c); }
    { Case c{"random dense 2e3/5e5", 2000, 0, 1999, {}, false}; F0R (i, 500000) c.ed.pb({R(0, 1999), R(0, 1999), R(1, 1000000000)}); cs.pb(c); }
    { int L = 40000; Case c{"bipartite unit 8e4/4e5", 2 * L + 2, 2 * L, 2 * L + 1, {}, false};
      F0R (i, L) { c.ed.pb({2 * L, i, 1}); c.ed.pb({L + i, 2 * L + 1, 1}); F0R (j, 8) c.ed.pb({i, L + R(0, L - 1), 1}); } cs.pb(c); }
    { int W = 400; Case c{"grid 400x400 bidir", W * W, 0, W * W - 1, {}, true};
      F0R (i, W) F0R (j, W) { if (i + 1 < W) c.ed.pb({i * W + j, (i + 1) * W + j, R(1, 1000000000)}); if (j + 1 < W) c.ed.pb({i * W + j, i * W + j + 1, R(1, 1000000000)}); } cs.pb(c); }
    { int Ly = 1500, Wd = 40; Case c{"layered 1500x40", Ly * Wd + 2, Ly * Wd, Ly * Wd + 1, {}, false};
      F0R (j, Wd) c.ed.pb({Ly * Wd, j, R(1, 1000000000)}), c.ed.pb({(Ly - 1) * Wd + j, Ly * Wd + 1, R(1, 1000000000)});
      F0R (l, Ly - 1) F0R (j, Wd) F0R (k, 5) c.ed.pb({l * Wd + j, (l + 1) * Wd + R(0, Wd - 1), R(1, 1000000000)}); cs.pb(c); }
    { Case c{"random unit 1e5/4e5", 100000, 0, 99999, {}, false}; F0R (i, 400000) c.ed.pb({R(0, 99999), R(0, 99999), 1}); cs.pb(c); }
    { Case c{"small caps 1e5/4e5", 100000, 0, 99999, {}, false}; F0R (i, 400000) c.ed.pb({R(0, 99999), R(0, 99999), R(1, 10)}); cs.pb(c); }
    return cs;
}
double now_ms() { return chrono::duration<double, milli>(chrono::steady_clock::now().time_since_epoch()).count(); }
int main() {
    auto cs = cases();
    for (Case &c : cs) {
        double best = 1e18; ll flow = -1;
        F0R (rep, 5) {
            FLOW_T fl; fl.init(c.n); for (auto [a, b, cap] : c.ed) fl.ae(a, b, cap, c.bidir ? cap : 0);
            double t0 = now_ms(); ll got = fl.calc(c.src, c.snk); best = min(best, now_ms() - t0);
            assert(flow == -1 || flow == got); flow = got;
        }
        printf("%-26s flow %-13lld %8.1f ms\n", c.name, flow, best);
    }
#ifdef INC
    { // incremental: raise random caps on the sparse graph, recompute each time
        Case &c = cs[0]; mt19937 rng(7);
        FLOW_T fl; fl.init(c.n); vt<pair<int,int>> ids;
        for (auto [a, b, cap] : c.ed) ids.pb({(int) a, size(fl.adj[a])}), fl.ae(a, b, cap, 0);
        ll flow = fl.calc(c.src, c.snk);
        double t0 = now_ms(); ll extra = 0;
        F0R (k, 500) { auto [a, i] = ids[rng() % size(ids)]; fl.adj[a][i].c += 1 + rng() % 1000000000; extra += fl.calc(c.src, c.snk); }
        printf("incremental: 500 raises + calc each   %8.1f ms total, %.2f ms per calc (flow %lld -> %lld)\n", now_ms() - t0, (now_ms() - t0) / 500, flow, flow + extra);
#ifdef LIM
        t0 = now_ms(); int done = 0; flow += extra;
        F0R (k, 5000) { auto [a, i] = ids[rng() % size(ids)]; auto &e = fl.adj[a][i]; auto &re = fl.adj[e.to][e.rev];
            ll f = re.c; if (!f) continue; ll nc = rng() % f, x = f - nc; e.c = 0, re.c -= x; // cap reduced to nc, below the flow
            ll z = x - fl.calc(a, e.to, x); fl.calc(a, c.src, z); fl.calc(c.snk, e.to, z); flow -= z; flow += fl.calc(c.src, c.snk); if (++done == 200) break; }
        printf("incremental: 200 lowers below flow   %8.1f ms total, %.2f ms per edit (flow now %lld)\n", now_ms() - t0, (now_ms() - t0) / 200, flow);
#endif
    }
#endif
}
