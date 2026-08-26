// Tests DSURollback.h: randomized unite/push/pop vs brute DSU rebuilt from
// scratch; checks unite()'s bool return, comps(), and pairwise connectivity.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/data-structures/DSURollback.h"

struct Brute {
    vi e;
    Brute(int n) : e(n, -1) {}
    int find(int x) { return e[x] < 0 ? x : find(e[x]); }
    bool unite(int x, int y) {
        x = find(x), y = find(y);
        if (x == y) return 0;
        e[x] = y;
        return 1;
    }
};

int main() {
    mt19937 rng(1234);
    F0R(cs, 20000) {
        int n = rng() % 8 + 1;
        DSU dsu;
        dsu.init(n);
        dsu.push();
        vt<vt<pi>> layers(1); // edges united per stack layer
        F0R(op, 40) {
            int c = rng() % 6;
            if (c == 0) { dsu.push(); layers.pb({}); }
            else if (c == 1 && size(layers) > 1) { dsu.pop(); layers.pop_back(); }
            else {
                int u = rng() % n, v = rng() % n;
                Brute b(n);
                for (auto& l : layers) for (auto [x, y] : l) b.unite(x, y);
                bool exp = b.unite(u, v);
                assert(dsu.unite(u, v) == exp);
                if (exp) layers.back().pb({u, v});
            }
            // verify comps + connectivity vs brute
            Brute b(n);
            int comps = n;
            for (auto& l : layers) for (auto [x, y] : l) comps -= b.unite(x, y);
            assert(dsu.comps() == comps);
            F0R(u, n) FOR(v, u + 1, n)
                assert((dsu.find(u) == dsu.find(v)) == (b.find(u) == b.find(v)));
        }
    }
    cout << "Tests passed!" << endl;
}
