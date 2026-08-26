// Tests GlobalMinCut (Stoer-Wagner, ll weights) against brute-force global
// min cut on random small graphs with weights up to 1e9 (overflow check);
// returned vertex set must realize the cut value. written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/graph/GlobalMinCut.h"

int main() {
    mt19937_64 rng(42);
    F0R (it, 5000) {
        int n = 2 + rng() % 6; // 2..7
        vt<vl> mat(n, vl(n));
        F0R (e, (int)(rng() % (n * n))) {
            int a = rng() % n, b = rng() % n;
            if (a == b) continue;
            ll w = rng() % 1000000001; // up to 1e9
            mat[a][b] += w; mat[b][a] += w;
        }
        ll best = LLONG_MAX; // brute global min cut
        FOR (mask, 1, (1 << n) - 1) {
            ll cut = 0;
            F0R (i, n) F0R (j, n)
                if ((mask >> i & 1) && !(mask >> j & 1)) cut += mat[i][j];
            best = min(best, cut);
        }
        auto [val, side] = globalMinCut(mat);
        assert(val == best);
        // side must be a nonempty proper subset realizing val
        vi in(n);
        assert(size(side) > 0 && size(side) < n);
        for (int x : side) {
            assert(0 <= x && x < n && !in[x]);
            in[x] = 1;
        }
        ll cut = 0;
        F0R (i, n) F0R (j, n) if (in[i] && !in[j]) cut += mat[i][j];
        assert(cut == val);
    }
    cout << "Tests passed!" << endl;
}
