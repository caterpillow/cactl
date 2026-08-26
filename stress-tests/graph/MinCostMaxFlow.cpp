// Tests MCMF (via MCMF_patched.h, a sed-derived copy of
// content/graph/MinCostMaxFlow.h with the extc++ include and setpi
// uncommented) against a Bellman-Ford successive-shortest-paths reference:
// nonnegative costs plain, negative costs (no negative cycles) with setpi.
// written by Claude (audit)
#include <bits/extc++.h>
#include "../utilities/template.h"
#include "MCMF_patched.h"

struct Ref { // Bellman-Ford SSP min-cost max-flow (handles negative costs)
    struct E { int to; ll cap, cost; };
    int n; vt<E> es; vt<vi> g;
    Ref(int n) : n(n), g(n) {}
    void ae(int a, int b, ll cap, ll cost) {
        g[a].pb(size(es)); es.pb({b, cap, cost});
        g[b].pb(size(es)); es.pb({a, 0, -cost});
    }
    pl run(int s, int t) {
        ll fl = 0, co = 0;
        while (true) {
            vl d(n, INF); vi pe(n, -1);
            d[s] = 0;
            F0R (it, n) F0R (u, n) if (d[u] < INF)
                for (int id : g[u]) {
                    auto& e = es[id];
                    if (e.cap > 0 && d[u] + e.cost < d[e.to])
                        d[e.to] = d[u] + e.cost, pe[e.to] = id;
                }
            if (d[t] >= INF) break;
            ll push = INF;
            for (int v = t; v != s; v = es[pe[v] ^ 1].to)
                push = min(push, es[pe[v]].cap);
            for (int v = t; v != s; v = es[pe[v] ^ 1].to) {
                es[pe[v]].cap -= push; es[pe[v] ^ 1].cap += push;
                co += push * es[pe[v]].cost;
            }
            fl += push;
        }
        return {fl, co};
    }
    bool negCycle() { // over positive-cap edges, any component
        vl d(n, 0);
        F0R (it, n + 1) {
            bool ch = false;
            F0R (u, n) for (int id : g[u]) {
                auto& e = es[id];
                if (e.cap > 0 && d[u] + e.cost < d[e.to])
                    d[e.to] = d[u] + e.cost, ch = true;
            }
            if (!ch) return false;
        }
        return true;
    }
};

int main() {
    mt19937 rng(7);
    F0R (it, 10000) {
        bool neg = it % 2; // half the cases allow negative costs
        int n = 2 + rng() % 5; // 2..6
        int s = rng() % n, t = rng() % (n - 1);
        if (t >= s) t++;
        MCMF mc(n);
        Ref ref(n);
        F0R (e, (int)(rng() % 11)) {
            int a = rng() % n, b = rng() % n;
            if (a == b) continue;
            ll cap = rng() % 11;
            ll cost = neg ? (ll)(rng() % 14) - 5 : rng() % 9;
            mc.ae(a, b, cap, cost);
            ref.ae(a, b, cap, cost);
        }
        if (ref.negCycle()) continue; // MCMF doesn't support negative cycles
        if (neg) mc.setpi(s);
        auto got = mc.maxflow(s, t);
        auto want = ref.run(s, t);
        assert(got == want);
    }
    cout << "Tests passed!" << endl;
}
