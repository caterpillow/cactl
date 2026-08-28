// Tests ConvexHull.h: convex_hull vs (a) the extreme-point definition on tiny
// inputs (a point is a hull vertex iff no segment/triangle of the other points
// contains it) and (b) an independent Jarvis march (CCW order, strict vertices
// only). Covers duplicates, collinear sets, 0/1/2 points. Written by Claude (audit).
#include "../utilities/template.h"
#include "../../content/geometry/Point.h"
using P = Point<ll>;
#include "../../content/geometry/ConvexHull.h"

mt19937 rng(20260829);
ll rnd(int C) { return (ll) (rng() % (2 * C + 1)) - C; }
vt<P> dedup(vt<P> p) { sort(all(p)); p.erase(unique(all(p)), p.end()); return p; }

vt<P> extreme(vt<P> p) { // strict hull vertices by definition, sorted
    p = dedup(p);
    vt<P> r;
    for (P q : p) {
        bool in = false;
        for (P a : p) for (P b : p) if (!(a == q) && !(b == q) && !(a == b)) {
            if (a.cross(b, q) == 0 && (a - q).dot(b - q) <= 0) in = true; // on segment ab
            for (P c : p) if (!(c == q) && a.cross(b, c) > 0) // q in CCW triangle abc
                if (a.cross(b, q) >= 0 && b.cross(c, q) >= 0 && c.cross(a, q) >= 0) in = true;
        }
        if (!in) r.pb(q);
    }
    return r;
}

vt<P> jarvis(vt<P> p) { // CCW from the lexicographic minimum, strict vertices only
    p = dedup(p);
    if (size(p) <= 2) return p;
    vt<P> h; P cur = p[0];
    do {
        h.pb(cur);
        P nx = cur == p[0] ? p[1] : p[0];
        for (P q : p) { // pick the rightmost direction, farthest among collinear
            ll c = cur.cross(nx, q);
            if (c < 0 || (c == 0 && (q - cur).dist2() > (nx - cur).dist2())) nx = q;
        }
        cur = nx;
        assert(size(h) <= size(p));
    } while (!(cur == p[0]));
    return h;
}

int main() {
    const int Cs[] = {1, 3, 1000};
    F0R (it, 60000) {
        int n = it % 2 ? (int) (rng() % 8) : (int) (rng() % 60) + 1, C = Cs[it % 3];
        vt<P> p(n);
        for (P &q : p) q = {rnd(C), rnd(C)};
        vt<P> h = convex_hull(p);
        if (!h.empty()) rotate(h.begin(), min_element(all(h)), h.end());
        assert(h == jarvis(p));
        if (n <= 7) assert(dedup(h) == extreme(p));
    }
    puts("Tests passed!");
}
