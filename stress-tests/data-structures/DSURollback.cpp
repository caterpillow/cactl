// Tests DSURollback.h (constructor + flat update log with checkpoints, 2026-10):
// random unite/push/pop against a stack of brute-force DSU snapshots; checks
// unite()'s return value, component count n - size(upds), and pairwise
// connectivity after every operation. written by Claude (audit)
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
    int comps() { int c = 0; for (int v : e) c += v < 0; return c; }
};

int main() {
    mt19937 rng(1234);
    F0R(it, 400) {
        int n = (int) (rng() % 12) + 1;
        DSU d(n);
        vt<Brute> snap; // snap.back() = brute state at the last push
        Brute cur(n);
        F0R(op, 400) {
            int r = (int) (rng() % 10);
            if (r < 6) {
                int x = (int) (rng() % n), y = (int) (rng() % n);
                assert(d.unite(x, y) == cur.unite(x, y));
            } else if (r < 8) {
                d.push(); snap.pb(cur);
            } else if (!snap.empty()) {
                d.pop(); cur = snap.back(); snap.pop_back();
            }
            assert(n - size(d.upds) == cur.comps());
            F0R(a, n) F0R(b, n) assert((d.find(a) == d.find(b)) == (cur.find(a) == cur.find(b)));
        }
    }
    cout << "Tests passed!" << endl;
}
