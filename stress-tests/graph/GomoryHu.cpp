// Tests GomoryHu (PushRelabel init/ae/calc/leftOfMinCut API): all-pairs min
// cut via tree path minimum must equal direct Dinic max flow. written by
// Claude (audit)
#include "../utilities/template.h"
#include "../../content/graph/GomoryHu.h"
#include "../../content/graph/Dinic.h"

int main() {
    mt19937 rng(999);
    F0R (it, 2000) {
        int n = 2 + rng() % 7; // 2..8
        vt<Edge> ed;
        F0R (e, (int)(rng() % (2 * n))) {
            ll a = rng() % n, b = rng() % n;
            if (a == b) continue;
            ed.pb({a, b, (ll)(rng() % 10)});
        }
        auto tree = gomoryHu(n, ed);
        assert(size(tree) == n - 1);
        vt<vt<pl>> g(n);
        for (auto& e : tree) {
            g[e[0]].pb({e[1], e[2]});
            g[e[1]].pb({e[0], e[2]});
        }
        F0R (src, n) {
            vl mn(n, -1);
            auto dfs = [&](auto&& self, int u, int p, ll cur) -> void {
                mn[u] = cur;
                for (auto [v, w] : g[u])
                    if (v != p) self(self, v, u, min(cur, w));
            };
            dfs(dfs, src, -1, LLONG_MAX);
            FOR (v, src + 1, n) {
                Dinic din; din.init(n);
                for (auto& e : ed) din.ae(e[0], e[1], e[2], e[2]);
                assert(mn[v] == din.calc(src, v));
            }
        }
    }
    cout << "Tests passed!" << endl;
}
