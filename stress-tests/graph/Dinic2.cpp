// Tests Dinic2.h against brute-force min cut on random small graphs (rcap,
// parallel edges, unreachable t), checks left_of_min_cut gives a cut of
// capacity == flow, that calc(s, t, lim) stops at lim, and the edit rules
// from the header: raising a cap then calc(s, t), and lowering one below
// its flow with the calc(a, b, x) / calc(a, s, z) / calc(t, b, z) recipe.
// Every edited graph is compared with a brute-force max flow.
// written by Claude (audit)
#include "../utilities/template.h"

#include "../../content/graph/Dinic2.h"

mt19937 rng(7);
ll brute(vt<vl> &cap, int s, int t) { // min s-t cut by enumeration
    int n = size(cap); ll best = LLONG_MAX;
    F0R (mask, 1 << n) {
        if (!(mask >> s & 1) || (mask >> t & 1)) continue;
        ll cut = 0;
        F0R (i, n) F0R (j, n) if ((mask >> i & 1) && !(mask >> j & 1)) cut += cap[i][j];
        best = min(best, cut);
    }
    return best;
}
struct Rec { int a, i, b; ll c, rc; }; // forward edge i of a, caps c / rc

void check_cut(Dinic &d, vt<vl> &cap, int s, int t, ll flow) {
    int n = size(cap); ll across = 0;
    assert(d.left_of_min_cut(s) && !d.left_of_min_cut(t));
    F0R (i, n) F0R (j, n) if (d.left_of_min_cut(i) && !d.left_of_min_cut(j)) across += cap[i][j];
    assert(across == flow);
}
int main() {
    F0R (it, 30000) {
        int n = 2 + rng() % 6, s = rng() % n, t = rng() % (n - 1); if (t >= s) t++;
        vt<vl> cap(n, vl(n)); Dinic d; d.init(n); vt<Rec> es;
        F0R (e, (int)(rng() % 14)) {
            int a = rng() % n, b = rng() % n; if (a == b) continue;
            ll c = rng() % 11, rc = rng() % 4 == 0 ? rng() % 11 : 0;
            cap[a][b] += c, cap[b][a] += rc;
            es.pb({a, size(d.adj[a]), b, c, rc}), d.ae(a, b, c, rc);
        }
        ll flow = d.calc(s, t);
        assert(flow == brute(cap, s, t));
        check_cut(d, cap, s, t, flow);
        { // lim: a fresh copy stops at lim, and the rest comes later
            Dinic d2; d2.init(n); for (Rec &r : es) d2.ae(r.a, r.b, r.c, r.rc);
            ll lim = rng() % 5, got = d2.calc(s, t, lim);
            assert(got == min(lim, flow) && got + d2.calc(s, t) == flow);
        }
        F0R (round, 3) if (size(es)) { // edits, then recompute
            Rec &r = es[rng() % size(es)];
            Dinic::Edge &e = d.adj[r.a][r.i], &re = d.adj[r.b][e.rev];
            if (rng() % 2) { // raise
                ll dd = rng() % 11;
                e.c += dd, cap[r.a][r.b] += dd, r.c += dd;
                flow += d.calc(s, t);
            } else { // lower to nc, maybe below the current flow f
                ll f = r.c - e.c, nc = rng() % (r.c + 1);
                cap[r.a][r.b] -= r.c - nc;
                if (f <= nc) e.c -= r.c - nc;
                else {
                    ll x = f - nc; e.c = 0, re.c -= x;
                    ll z = x - d.calc(r.a, r.b, x);
                    assert(d.calc(r.a, s, z) == z && d.calc(t, r.b, z) == z);
                    flow -= z, flow += d.calc(s, t);
                }
                r.c = nc;
            }
            // state must be a valid flow: residual sums and non-negativity
            for (Rec &q : es) { Dinic::Edge &x = d.adj[q.a][q.i], &y = d.adj[q.b][x.rev];
                assert(x.c >= 0 && y.c >= 0 && x.c + y.c == q.c + q.rc); }
            assert(d.calc(s, t) == 0); // already maximum
            assert(flow == brute(cap, s, t));
        }
        flow += d.calc(s, t); check_cut(d, cap, s, t, flow);
    }
    cout << "Tests passed!" << endl;
}
