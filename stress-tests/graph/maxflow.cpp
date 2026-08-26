// Tests Dinic, EdmondsKarp, PushRelabel and FordFulkerson against brute-force
// min cut on random small graphs; also checks PushRelabel.leftOfMinCut gives a
// cut of capacity == flow. written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/graph/Dinic.h"
#include "../../content/graph/EdmondsKarp.h"
#include "../../content/graph/PushRelabel.h"
#include "../../content/graph/FordFulkerson.h"

int main() {
    mt19937 rng(1234);
    F0R (it, 20000) {
        int n = 2 + rng() % 6; // 2..7
        int s = rng() % n, t = rng() % (n - 1);
        if (t >= s) t++;
        vt<vl> cap(n, vl(n));
        Dinic din; din.init(n);
        PushRelabel<ll> pr; pr.init(n);
        vt<unordered_map<int, ll>> ek(n);
        F0R (i, n) ::adj[i].clear();
        F0R (e, (int)(rng() % 14)) {
            int a = rng() % n, b = rng() % n;
            if (a == b) continue;
            ll c = rng() % 11, d = rng() % 4 == 0 ? rng() % 11 : 0;
            cap[a][b] += c; cap[b][a] += d;
            din.ae(a, b, c, d);
            pr.ae(a, b, c, d);
            ek[a][b] += c; ek[b][a] += d;
            ::adj[a][b] += (int)c; ::adj[b][a] += (int)d;
        }
        ll best = LLONG_MAX; // brute min s-t cut
        F0R (mask, 1 << n) {
            if (!(mask >> s & 1) || (mask >> t & 1)) continue;
            ll cut = 0;
            F0R (i, n) F0R (j, n)
                if ((mask >> i & 1) && !(mask >> j & 1)) cut += cap[i][j];
            best = min(best, cut);
        }
        ll fd = din.calc(s, t), fp = pr.calc(s, t);
        ll fe = edmondsKarp(ek, s, t), ff = ::flow(s, t);
        assert(fd == best && fp == best && fe == best && ff == best);
        // PushRelabel's min cut must be a valid cut of capacity == flow
        assert(pr.leftOfMinCut(s) && !pr.leftOfMinCut(t));
        ll across = 0;
        F0R (i, n) F0R (j, n)
            if (pr.leftOfMinCut(i) && !pr.leftOfMinCut(j)) across += cap[i][j];
        assert(across == fp);
    }
    cout << "Tests passed!" << endl;
}
