// Tests SegmentIntersection.h (seg_inter) on a small integer grid (exact in
// doubles) against the old KACTL segmentIntersection: same count (0/1/2) and
// the same point(s); every returned point is also checked to lie on both
// segments via seg_dist.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/geometry/SegmentIntersection.h"
using P = Point<db>;
#include "../../content/geometry/SegmentDistance.h"

namespace ref { // old KACTL implementation, independent of seg_inter
int segInter(P s1, P e1, P s2, P e2, P& r1, P& r2) {
    if (e1 == s1) {
        if (e2 == s2) {
            if (e1 == e2) { r1 = e1; return 1; }
            return 0;
        }
        return segInter(s2, e2, s1, e1, r1, r2);
    }
    P v1 = e1 - s1, v2 = e2 - s2, d = s2 - s1;
    auto a = v1.cross(v2), a1 = v1.cross(d), a2 = v2.cross(d);
    if (a == 0) { // parallel
        auto b1 = s1.dot(v1), c1 = e1.dot(v1), b2 = s2.dot(v1), c2 = e2.dot(v1);
        if (a1 || a2 || max(b1, min(b2, c2)) > min(c1, max(b2, c2))) return 0;
        r1 = min(b2, c2) < b1 ? s1 : (b2 < c2 ? s2 : e2);
        r2 = max(b2, c2) > c1 ? e1 : (b2 > c2 ? s2 : e2);
        return 2 - (r1 == r2);
    }
    if (a < 0) a = -a, a1 = -a1, a2 = -a2;
    if (0 < a1 || a < -a1 || 0 < a2 || a < -a2) return 0;
    r1 = s1 - v1 * a2 / a;
    return 1;
}
}

bool eq(P a, P b) { return (a - b).dist() < 1e-8; }

int main() {
    mt19937 rng(1337);
    const int GRID = 6;
    F0R (t, 500000) {
        auto r = [&] { return (db) (int) (rng() % GRID); };
        P a{r(), r()}, b{r(), r()}, c{r(), r()}, d{r(), r()};
        P t1{}, t2{};
        int n = ref::segInter(a, b, c, d, t1, t2);
        vt<P> res = seg_inter(a, b, c, d);
        assert(n == size(res));
        for (P p : res) assert(seg_dist(a, b, p) < 1e-8 && seg_dist(c, d, p) < 1e-8);
        if (n == 1) assert(eq(res[0], t1));
        if (n == 2) { // set<P> is sorted by (x, y)
            if (t2 < t1) swap(t1, t2);
            assert(eq(res[0], t1) && eq(res[1], t2));
        }
    }
    cout << "Tests passed!" << endl;
}
