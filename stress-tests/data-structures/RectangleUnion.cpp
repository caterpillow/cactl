// Tests RectangleUnion.h: rect_union() against a brute-force grid count on
// small coordinates, with degenerate (empty/inverted), duplicate, nested and
// touching rectangles; also reuses the global tree across calls and checks a
// large-y case. written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/data-structures/RectangleUnion.h"

ll brute(const vt<array<int, 4>> &R, int C) {
    vt<vi> g(C, vi(C, 0));
    for (auto [x1, y1, x2, y2] : R)
        for (int x = x1; x < x2; x++)
            for (int y = y1; y < y2; y++) g[x][y] = 1;
    ll res = 0;
    F0R(x, C) F0R(y, C) res += g[x][y];
    return res;
}

int main() {
    mt19937 rng(20261006);
    auto rnd = [&](int n) { return (int) (rng() % (unsigned) n); };
    assert(rect_union({}) == 0);
    assert(rect_union({{0, 0, 5, 5}}) == 25);
    assert(rect_union({{3, 3, 3, 9}, {4, 4, 2, 8}, {1, 5, 7, 5}}) == 0);
    F0R(it, 3000) {
        int C = it % 3 == 0 ? 6 : 40; // small grids force heavy overlap
        int n = rnd(it % 5 == 0 ? 40 : 8);
        vt<array<int, 4>> R;
        F0R(i, n) {
            int t = rnd(8);
            if (t == 0 && size(R)) { R.pb(R[rnd(size(R))]); continue; } // duplicate
            if (t == 1 && size(R)) { // nested in / touching an existing one
                auto [a, b, c, d] = R[rnd(size(R))];
                if (a < c && b < d) {
                    int x1 = a + rnd(c - a), y1 = b + rnd(d - b);
                    R.pb({x1, y1, x1 + 1 + rnd(c - x1), y1 + 1 + rnd(d - y1)});
                } else R.pb({c, d, c + 1, d + 1});
                continue;
            }
            if (t == 2 && size(R)) { // adjacent to an existing one
                auto [a, b, c, d] = R[rnd(size(R))];
                if (c < C && b < d && d <= C) R.pb({c, b, min(C, c + 1 + rnd(5)), d});
                continue;
            }
            int x1 = rnd(C), x2 = rnd(C), y1 = rnd(C), y2 = rnd(C); // may be degenerate
            if (t >= 5 && x1 > x2) swap(x1, x2);
            if (t >= 6 && y1 > y2) swap(y1, y2);
            R.pb({x1, y1, x2, y2});
        }
        assert(rect_union(R) == brute(R, C));
    }
    // large coordinates near MY, overflow of 32-bit area
    int hi = MY;
    assert(rect_union({{0, 0, 1000000000, hi}}) == 1000000000LL * hi);
    assert(rect_union({{0, 0, 1000000000, hi}, {5, 7, 900000000, hi - 3}}) == 1000000000LL * hi);
    assert(rect_union({{0, 0, 10, hi / 2}, {0, hi / 2, 10, hi}}) == 10LL * hi);
    assert(rect_union({{0, 0, 10, hi / 2}, {10, hi / 2, 20, hi}}) == 10LL * hi);
    cout << "Tests passed!" << endl;
}
