// Tests FastMo.h: Q=0 construct+run must not crash (climb guard), then
// randomized distinct-count queries vs brute. written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/data-structures/FastMo.h"

int main() {
    mt19937 rng(5);
    { // Q = 0
        Fast_Mo mo(50, 0);
        mo.run([](int){}, [](int){}, [](int){}, [](int){}, [](int){});
    }
    F0R (round, 20) {
        int n = 1 + rng() % 200, Q = 1 + rng() % 100;
        vi a(n);
        for (int& x : a) x = rng() % 10;
        Fast_Mo mo(n, Q);
        vt<pi> qs;
        F0R (q, Q) {
            int l = rng() % (n + 1), r = rng() % (n + 1);
            if (l > r) swap(l, r);
            qs.pb({l, r});
            mo.insert(l, r);
        }
        vi cnt(10), res(Q);
        int distinct = 0;
        auto addv = [&](int i) { if (!cnt[a[i]]++) distinct++; };
        auto delv = [&](int i) { if (!--cnt[a[i]]) distinct--; };
        mo.run(addv, addv, delv, delv, [&](int q) { res[q] = distinct; });
        F0R (q, Q) {
            vi c(10);
            int want = 0;
            FOR (k, qs[q].f, qs[q].s) if (!c[a[k]]++) want++;
            assert(res[q] == want);
        }
    }
    cout << "Tests passed!" << endl;
}
