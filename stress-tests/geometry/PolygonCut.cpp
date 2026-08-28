// Tests polygonCut on random simple polygons (integer coords, convex and
// non-convex) against an independent brute force: fan-triangulate the polygon,
// clip each triangle with a differently derived half-plane clip and sum the
// signed areas. Also checks every output vertex is a kept input vertex (in
// order) or an on-line point lying on an input edge, and that the two halves
// (cut by s->e and by e->s) add up to the whole polygon. Written by Claude (audit).
#include "../utilities/template.h"

#include "../../content/geometry/PolygonArea.h"
#include "../../content/geometry/PolygonCut.h"
#include "../../content/geometry/sideOf.h"
#include "../../content/geometry/SegmentDistance.h"
#include "../../content/geometry/SegmentIntersection.h"

mt19937 rng(1234);
const db EPS = 1e-9;
P A, B; // cut line A -> B; keep the right side

db side(P p) { return (B - A).cross(p - A); }

// twice the signed area of triangle abc restricted to the right of A->B,
// intersections parametrised along the cut line (unlike the header)
db tri_cut2(P a, P b, P c) {
    P t[3] = {a, b, c};
    vt<P> q;
    F0R (i, 3) {
        P u = t[i], v = t[(i + 1) % 3];
        if (side(u) <= 0) q.pb(u);
        if (side(u) * side(v) < 0)
            q.pb(A + (B - A) * ((u - A).cross(v - u) / (B - A).cross(v - u)));
    }
    return size(q) < 3 ? 0 : polygon_area(q);
}

bool simple(vt<P>& p) {
    int n = size(p);
    F0R (i, n) FOR (j, i + 1, n) {
        bool adj = j == i + 1 || (i == 0 && j == n - 1);
        if (size(seg_inter(p[i], p[(i + 1) % n], p[j], p[(j + 1) % n])) != adj)
            return false;
    }
    return true;
}

P rnd_pt() {
    uniform_int_distribution<int> c(-5, 5);
    return {(db) c(rng), (db) c(rng)};
}

db area(vt<P>& p) { return p.empty() ? 0 : polygon_area(p); }

int main() {
    int done = 0;
    while (done < 3000) {
        int n = (int) (rng() % 10) + 3;
        vt<P> p(n);
        for (P& x : p) x = rnd_pt();
        if (rng() % 2) { // star-shaped around the centroid: more non-convex ones
            P c{0, 0};
            for (P x : p) c = c + x / (db) n;
            sort(all(p), [&](P a, P b) { return (a - c).angle() < (b - c).angle(); });
        }
        if (!simple(p)) continue;
        A = rnd_pt(), B = rnd_pt();
        if (A == B) continue;
        done++;

        vt<P> res = polygonCut(p, A, B), other = polygonCut(p, B, A);

        db expect = 0;
        FOR (i, 1, n - 1) expect += tri_cut2(p[0], p[i], p[i + 1]);
        assert(abs(area(res) - expect) < 1e-6);
        assert(abs(area(res) + area(other) - area(p)) < 1e-6);

        vt<P> kept;
        for (P x : p) if (side_of(A, B, x, EPS) < 0) kept.pb(x);
        int k = 0;
        for (P r : res) {
            int sd = side_of(A, B, r, EPS);
            assert(sd <= 0);
            if (sd < 0) { assert(k < size(kept) && r == kept[k]); k++; }
            else {
                bool on = false;
                F0R (i, n) on |= seg_dist(p[i], p[(i + 1) % n], r) < EPS;
                assert(on);
            }
        }
        assert(k == size(kept));
    }
    cout << "Tests passed!" << endl;
}
