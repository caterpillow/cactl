// Tests ManhattanMST: candidate edge set (<= 4N edges, correct weights) yields
// the same MST cost as O(N^2) Kruskal on all pairs; random points incl. heavy
// duplicates/ties and near-limit coordinates. Written by Claude (audit).
#include "../utilities/template.h"
#undef sz // UnionFind.h has a member named sz

#include "../../content/geometry/Point.h"
#include "../../content/geometry/ManhattanMST.h"
#include "../../content/data-structures/UnionFind.h"

mt19937 rng(1337);
int rnd(int lo, int hi) { return uniform_int_distribution<int>(lo, hi)(rng); }

ll dist(P a, P b) { return abs((ll) a.x - b.x) + abs((ll) a.y - b.y); }

ll brute(const vt<P>& ps) {
    int n = size(ps);
    vt<pair<ll, pi>> es;
    F0R (i, n) FOR (j, i + 1, n) es.pb({dist(ps[i], ps[j]), {i, j}});
    sort(all(es));
    UF uf(n);
    ll cost = 0;
    for (auto& e : es) if (uf.join(e.s.f, e.s.s)) cost += e.f;
    return cost;
}

int main() {
    F0R (t, 6000) {
        int c = t % 3 == 0 ? rnd(1, 4) : t % 3 == 1 ? rnd(1, 300) : rnd(1, 400000000);
        int n = t % 5 == 4 ? rnd(0, 300) : rnd(0, 60);
        if (c > 1000) n = min(n, 40);
        vt<P> ps;
        F0R (i, n) ps.pb(P{rnd(-c, c), rnd(-c, c)});
        auto edges = manhattanMST(ps);
        assert(size(edges) <= 4 * n);
        sort(all(edges));
        UF uf(n);
        ll cost = 0;
        int joined = 0;
        for (auto& e : edges) {
            assert(e[1] != e[2] && e[0] == dist(ps[e[1]], ps[e[2]]));
            if (uf.join(e[1], e[2])) cost += e[0], joined++;
        }
        assert(joined == max(n - 1, 0));
        assert(cost == brute(ps));
    }
    cout << "Tests passed!" << endl;
}
