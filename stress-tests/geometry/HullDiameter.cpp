// Tests hull_diameter (via convex_hull): the squared diameter must equal the
// brute-force max pairwise dist2 over all input points, and both returned
// points must be input points. Small grids force duplicates/collinear input.
// Written by Claude (audit).
#include "../utilities/template.h"

#include "../../content/geometry/Point.h"
using P = Point<ll>;
#include "../../content/geometry/ConvexHull.h"
#include "../../content/geometry/HullDiameter.h"

int main() {
    mt19937 rng(2);
    auto rnd = [&](int lo, int hi) { return (int) (rng() % (hi - lo + 1)) + lo; };
    F0R (it, 300000) {
        int n = rnd(1, 12), g = rnd(1, 6);
        vt<P> ps(n);
        for (P& p : ps) p = {rnd(-g, g), rnd(-g, g)};
        ll want = 0;
        F0R (i, n) F0R (j, i) want = max(want, (ps[i] - ps[j]).dist2());
        auto pa = hull_diameter(convex_hull(ps));
        assert((pa[0] - pa[1]).dist2() == want);
        for (P p : pa) assert(count(all(ps), p));
    }
    cout << "Tests passed!" << endl;
}
