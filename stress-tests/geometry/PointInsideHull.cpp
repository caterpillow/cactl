// Tests PointInsideHull.h (in_hull): on random hulls from convex_hull (tiny
// grid for degenerate size-1/2 hulls, small grid, and ~300-point sets on a
// 1000-grid for long binary searches) strict and non-strict answers must
// match a half-plane brute force for random points, every hull vertex and a
// lattice point on every hull edge.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/geometry/Point.h"
using P = Point<ll>;
#include "../../content/geometry/ConvexHull.h"
#include "../../content/geometry/PointInsideHull.h"

// convex CCW polygon: inside iff left of (or on, if !strict) every edge
bool brute(const vt<P>& h, P p, bool strict) {
    int n = size(h);
    if (n == 1) return !strict && p == h[0];
    if (n == 2) return !strict && on_segment(h[0], h[1], p);
    F0R (i, n) if (sgn(h[i].cross(h[(i + 1) % n], p)) < strict) return false;
    return true;
}

int main() {
    mt19937 rng(2024);
    auto r = [&](int g) { return (ll) (rng() % (2 * g + 1)) - g; };
    auto run = [&](int iters, int maxn, int grid) {
        F0R (it, iters) {
            vt<P> ps;
            F0R (i, (int) (rng() % maxn) + 1) ps.pb({r(grid), r(grid)});
            vt<P> h = convex_hull(ps), qs = h;
            F0R (i, 20) qs.pb({r(grid + 1), r(grid + 1)});
            F0R (i, size(h)) { // lattice point on edge i
                P d = h[(i + 1) % size(h)] - h[i];
                ll g = __gcd(abs(d.x), abs(d.y));
                if (g) qs.pb(h[i] + d / g);
            }
            for (P p : qs) for (bool strict : {false, true})
                assert(in_hull(h, p, strict) == brute(h, p, strict));
        }
    };
    run(50000, 4, 2);
    run(50000, 15, 10);
    run(2000, 300, 1000);
    cout << "Tests passed!" << endl;
}
