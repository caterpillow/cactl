// Tests GabowMatching.h (incl. its greedy warm start): matching size vs subset-DP brute (n <= 10, with
// self-loops and multi-edges) and vs Blossom.h on structured graphs up to
// n = 600 (odd-cycle chains, flowers, dense, bipartite); mate[] must be a
// valid matching of graph edges; one global instance reused via init().
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/graph/GabowMatching.h"
#include "../../content/graph/Blossom.h"
#include <functional>

int brute(int n, vt<pi>& es) {
    vi memo(1 << n, -1);
    function<int(int)> go = [&](int m) -> int {
        int &r = memo[m]; if (r != -1) return r; r = 0;
        for (auto [u, v] : es) if (u != v && (m >> u & 1) && (m >> v & 1))
            r = max(r, 1 + go(m ^ 1 << u ^ 1 << v));
        return r;
    };
    return go((1 << n) - 1);
}

GabowMatching G;
mt19937 rng(2026);
int R(int k) { return (int) (rng() % (unsigned) k); }

void check(int n, vt<pi>& es, int want) {
    G.init(n);
    for (auto [u, v] : es) G.ae(u + 1, v + 1);
    int got = G.solve(), cnt = 0;
    set<pi> E;
    for (auto [u, v] : es) E.insert({u + 1, v + 1}), E.insert({v + 1, u + 1});
    FOR(v, 1, n + 1) if (G.mate[v]) {
        int w = G.mate[v];
        assert(w != v && G.mate[w] == v && E.count({v, w}));
        cnt++;
    }
    assert(cnt == 2 * got && got == want);
}

vt<pi> gen(int n, int kind) {
    vt<pi> es;
    if (kind == 0) F0R(i, R(3 * n + 1)) es.pb({R(n), R(n)});
    if (kind == 1) F0R(i, n) F0R(j, i) if (R(100) < 30) es.pb({i, j});
    if (kind == 2) { // chain of triangles/pentagons: nested blossoms
        vi p(n); iota(all(p), 0); shuffle(all(p), rng);
        for (int i = 0; i + 2 < n;) {
            int len = R(2) && i + 5 <= n ? 5 : 3;
            F0R(j, len) es.pb({p[i + j], p[i + (j + 1) % len]});
            if (i) es.pb({p[i], p[R(i)]});
            i += len;
        }
    }
    if (kind == 3) F0R(i, 2 * n) es.pb({R(n / 2), n / 2 + R(n - n / 2)});
    if (kind == 4) { // random tree plus chords
        FOR(i, 1, n) es.pb({R(i), i});
        F0R(i, n / 2) es.pb({R(n), R(n)});
    }
    return es;
}

int main() {
    F0R(it, 60000) {
        int n = R(11), m = n ? R(n * (n + 1) / 2 + 3) : 0;
        vt<pi> es;
        F0R(i, m) es.pb({R(n), R(n)});
        check(n, es, brute(n, es));
    }
    F0R(it, 1500) {
        int n = 2 + R(600), kind = it % 5;
        vt<pi> es = gen(n, kind);
        Blossom B; B.init(n);
        for (auto [u, v] : es) if (u != v) B.ae(u + 1, v + 1);
        check(n, es, B.solve());
    }
    cout << "Tests passed!" << endl;
}
