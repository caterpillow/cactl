// Tests NetworkSimplex: compute() with zero edges returns 0 (run under
// -fsanitize=address), and random min-cost circulations (negative cost cycles
// allowed) vs brute-force cycle canceling. written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/graph/NetworkSimplex.h"

struct Cancel { // reference: negative-cycle canceling on residual graph
    struct E { int to; ll cap, cost; };
    int n; vt<E> es; vt<vi> g;
    Cancel(int n) : n(n), g(n) {}
    void ae(int a, int b, ll cap, ll cost) {
        g[a].pb(size(es)); es.pb({b, cap, cost});
        g[b].pb(size(es)); es.pb({a, 0, -cost});
    }
    ll run() {
        ll cost = 0;
        while (true) {
            vl d(n, 0); vi pe(n, -1);
            int bad = -1;
            F0R (it, n + 1) {
                bad = -1;
                F0R (u, n) for (int id : g[u]) {
                    auto& e = es[id];
                    if (e.cap > 0 && d[u] + e.cost < d[e.to])
                        d[e.to] = d[u] + e.cost, pe[e.to] = id, bad = e.to;
                }
                if (bad == -1) break;
            }
            if (bad == -1) return cost;
            F0R (i, n) bad = es[pe[bad] ^ 1].to; // land on the cycle
            vi cyc; // edge ids of the cycle
            for (int v = bad;;) {
                cyc.pb(pe[v]);
                v = es[pe[v] ^ 1].to;
                if (v == bad) break;
            }
            ll push = INF;
            for (int id : cyc) push = min(push, es[id].cap);
            for (int id : cyc) {
                es[id].cap -= push; es[id ^ 1].cap += push;
                cost += push * es[id].cost;
            }
        }
    }
};

int main() {
    // zero-edge guard: no ae() calls must return 0, not crash
    { NetworkSimplex ns(1); assert(ns.compute() == 0); }
    { NetworkSimplex ns(6); assert(ns.compute() == 0); }
    mt19937 rng(13);
    F0R (it, 5000) {
        int n = 1 + rng() % 6; // 1..6
        NetworkSimplex ns(n);
        Cancel ref(n);
        F0R (e, (int)(rng() % 9)) {
            int a = rng() % n, b = rng() % n;
            if (a == b) continue;
            ll cap = rng() % 6, cost = (ll)(rng() % 21) - 10;
            ns.ae(a, b, cap, cost);
            ref.ae(a, b, cap, cost);
        }
        ll got = ns.compute(), want = ref.run();
        assert(got == want);
    }
    cout << "Tests passed!" << endl;
}
