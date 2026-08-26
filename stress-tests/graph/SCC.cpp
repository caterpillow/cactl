// Tests SCC.h (Kosaraju) and SCC2.h (Tarjan) vs brute-force SCC on random
// small digraphs: comp[] partition, comps = one rep per SCC, topological order.
// written by Claude (audit)
#include "../utilities/template.h"

namespace k1 {
#include "../../content/graph/SCC.h"
}
namespace k2 {
#include "../../content/graph/SCC2.h"
}

mt19937 rng(12345);

// check one implementation's output against brute same-SCC relation
void check(int n, vt<vi>& adj, vt<vi>& same, vi& comp, vi& comps) {
    F0R (u, n) F0R (v, n) assert((comp[u] == comp[v]) == same[u][v]);
    // comps: one rep per SCC, rep is its own comp id, all distinct
    set<int> reps(all(comps)), ids(all(comp));
    assert(size(comps) == size(reps) && reps == ids);
    for (int c : comps) assert(comp[c] == c);
    // topological order: every edge goes left to right in comps
    vi pos(n, -1);
    F0R (i, size(comps)) pos[comps[i]] = i;
    F0R (u, n) for (int v : adj[u]) assert(pos[comp[u]] <= pos[comp[v]]);
}

int main() {
    F0R (it, 100000) {
        int n = rng() % 8 + 1;
        vt<vi> adj(n);
        int m = rng() % (n * n + 1);
        vt<vt<bool>> reach(n, vt<bool>(n));
        F0R (i, n) reach[i][i] = 1;
        F0R (e, m) {
            int u = rng() % n, v = rng() % n; // self loops + dups allowed
            adj[u].pb(v);
            reach[u][v] = 1;
        }
        F0R (k, n) F0R (i, n) F0R (j, n)
            if (reach[i][k] && reach[k][j]) reach[i][j] = 1;
        vt<vi> same(n, vi(n));
        F0R (i, n) F0R (j, n) same[i][j] = reach[i][j] && reach[j][i];
        k1::SCC s1(adj);
        k2::SCC s2(n, adj);
        check(n, adj, same, s1.comp, s1.comps);
        check(n, adj, same, s2.comp, s2.comps);
    }
    cout << "Tests passed!" << endl;
}
