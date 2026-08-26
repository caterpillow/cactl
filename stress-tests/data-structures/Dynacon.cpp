// Tests Dynacon.h: random edge toggle/query timelines (n<=8, 40 ops) vs brute
// component counting via DFS at each query. 20k cases.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/data-structures/Dynacon.h"

int brute_comps(int n, set<pi>& eds) {
    vi vis(n);
    int c = 0;
    F0R(i, n) if (!vis[i]) {
        c++;
        vi st{i};
        vis[i] = 1;
        while (size(st)) {
            int u = st.back(); st.pop_back();
            F0R(v, n) if (!vis[v] && (eds.count({min(u, v), max(u, v)})))
                vis[v] = 1, st.pb(v);
        }
    }
    return c;
}

int main() {
    mt19937 rng(4321);
    F0R(cs, 20000) {
        int n = rng() % 8 + 1, q = 40;
        DynaCon dc;
        dc.init(n, q);
        set<pi> eds;
        vi expect;
        F0R(op, q) {
            if (rng() % 3 == 0) {
                dc.query();
                expect.pb(brute_comps(n, eds));
            } else {
                int u = rng() % n, v = rng() % n;
                if (u == v) { dc.query(); expect.pb(brute_comps(n, eds)); continue; }
                if (u > v) swap(u, v);
                dc.toggle(u, v);
                if (eds.count({u, v})) eds.erase({u, v});
                else eds.insert({u, v});
            }
        }
        assert(dc.ans() == expect);
    }
    cout << "Tests passed!" << endl;
}
