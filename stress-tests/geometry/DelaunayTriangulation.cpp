// Delaunay triangulation vs brute force. Written by Claude (audit).
// Random integer point sets in general position (no duplicate, collinear triple
// or cocircular quadruple - all checked with exact integer arithmetic). Every
// emitted triangle must be CCW with an exactly empty circumcircle, every point
// must be used, #triangles must equal 2n - h - 2, and the triangle areas must
// sum to the hull area (convex_hull + polygon_area). cc_center / cc_radius are
// cross-checked against the exact in-circle test.
#include "../utilities/template.h"
#include "../../content/geometry/DelaunayTriangulation.h"
#include "../../content/geometry/circumcircle.h" // defines P = Point<db>
#include "../../content/geometry/ConvexHull.h"
#include "../../content/geometry/PolygonArea.h"

// exact: > 0 iff d is strictly inside the circumcircle of CCW triangle (a,b,c)
ll incircle(P a, P b, P c, P d) {
    ll m[3][3];
    P v[3] = {a - d, b - d, c - d};
    F0R(i, 3) {
        m[i][0] = (ll) v[i].x; m[i][1] = (ll) v[i].y;
        m[i][2] = m[i][0] * m[i][0] + m[i][1] * m[i][1];
    }
    return m[0][0] * (m[1][1] * m[2][2] - m[1][2] * m[2][1])
         - m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0])
         + m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);
}

int main() {
    feenableexcept(29);
    mt19937 rng(20260829);
    auto rnd = [&](int n) { return (int) (rng() % (unsigned) n); };
    for (int done = 0; done < 20000;) {
        int n = rnd(20) + 1;
        vt<P> ps;
        F0R(i, n) ps.pb(P{(db) (rnd(100) - 50), (db) (rnd(100) - 50)});
        bool bad = false;
        F0R(i, n) F0R(j, i) if (ps[i] == ps[j]) bad = true;
        F0R(i, n) F0R(j, i) F0R(k, j)
            if (ps[i].cross(ps[j], ps[k]) == 0) bad = true;
        F0R(i, n) F0R(j, i) F0R(k, j) F0R(l, k)
            if (incircle(ps[i], ps[j], ps[k], ps[l]) == 0) bad = true;
        if (bad) continue;
        done++;
        db sumar = 0; int cnt = 0; vi used(n);
        delaunay(ps, [&](int i, int j, int k) {
            cnt++; used[i] = used[j] = used[k] = 1;
            db ar = ps[i].cross(ps[j], ps[k]);
            assert(ar > 0); // CCW
            sumar += ar;
            P c = cc_center(ps[i], ps[j], ps[k]);
            db r = cc_radius(ps[i], ps[j], ps[k]);
            for (int v : {i, j, k}) assert(abs((ps[v] - c).dist() - r) < 1e-6);
            F0R(l, n) if (l != i && l != j && l != k) {
                assert(incircle(ps[i], ps[j], ps[k], ps[l]) < 0);
                assert((ps[l] - c).dist() > r - 1e-6);
            }
        });
        vt<P> hull = convex_hull(ps);
        if (n >= 3) {
            F0R(i, n) assert(used[i]);
            assert(cnt == 2 * n - size(hull) - 2);
        } else assert(cnt == 0);
        assert(abs(sumar - polygon_area(hull)) < 0.5); // both exact integers
    }
    cout << "Tests passed!" << endl;
}
