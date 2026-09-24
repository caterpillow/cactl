// Tests Chordal.h: ok against brute-force recognition (repeatedly delete a
// simplicial vertex; chordal iff that empties the graph) on random graphs
// of every density and on random subtree-intersection graphs (always
// chordal), n <= 9; when ok, peo must be a permutation in which every
// vertex's later neighbours form a clique. Plus a 2e5-vertex path/clique
// chain for speed and a long cycle (not chordal). written by Claude (audit)
#include "../utilities/template.h"

#include "../../content/graph/Chordal.h"

mt19937 rng(11);
bool brute(int n, vt<vt<bool>> a) {
    vt<bool> gone(n);
    F0R (it, n) {
        int pick = -1;
        F0R (v, n) if (!gone[v] && pick < 0) {
            vi nb; F0R (u, n) if (!gone[u] && a[v][u]) nb.pb(u);
            bool cl = 1;
            for (int x : nb) for (int y : nb) if (x != y && !a[x][y]) cl = 0;
            if (cl) pick = v;
        }
        if (pick < 0) return 0;
        gone[pick] = 1;
    }
    return 1;
}
void check(int n, vt<vt<bool>> &a) {
    vt<vi> g(n); F0R (i, n) F0R (j, n) if (a[i][j]) g[i].pb(j);
    Chordal c(g);
    assert(c.ok == brute(n, a));
    if (!c.ok) return;
    vi at(n, -1); F0R (i, n) { assert(at[c.peo[i]] < 0); at[c.peo[i]] = i; }
    F0R (v, n) for (int x : g[v]) for (int y : g[v])
        if (x != y && at[x] > at[v] && at[y] > at[v]) assert(a[x][y]);
}
int main() {
    F0R (it, 200000) {
        int n = rng() % 10; vt<vt<bool>> a(n, vt<bool>(n));
        if (rng() % 2) { // random graph, random density
            int p = rng() % 101;
            F0R (i, n) F0R (j, i) if ((int) (rng() % 100) < p) a[i][j] = a[j][i] = 1;
        } else if (n) { // subtrees (paths) of a random tree intersect
            int k = rng() % 8 + 1; vi par(k); FOR (i, 1, k) par[i] = rng() % i;
            auto path = [&] (int x, int y) {
                vt<bool> on(k), ax(k); for (int z = x; ; z = par[z]) { ax[z] = 1; if (!z) break; }
                int l = y; while (!ax[l]) l = par[l];
                for (int z = x; z != l; z = par[z]) on[z] = 1;
                for (int z = y; z != l; z = par[z]) on[z] = 1;
                on[l] = 1; return on; };
            vt<vt<bool>> s(n); F0R (i, n) s[i] = path(rng() % k, rng() % k);
            F0R (i, n) F0R (j, i) F0R (z, k) if (s[i][z] && s[j][z]) a[i][j] = a[j][i] = 1;
        }
        check(n, a);
    }
    { // big: overlapping triangles in a chain are chordal, a long cycle is not
        int n = 200000; vt<vi> g(n);
        FOR (i, 1, n) { g[i].pb(i - 1), g[i - 1].pb(i); if (i > 1) g[i].pb(i - 2), g[i - 2].pb(i); }
        Chordal c(g); assert(c.ok && size(c.peo) == n);
        vt<vi> cy(n); F0R (i, n) cy[i].pb((i + 1) % n), cy[(i + 1) % n].pb(i);
        assert(!Chordal(cy).ok);
    }
    cout << "Tests passed!" << endl;
}
