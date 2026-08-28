// Tests CircleLine.h (circle_line): known values; integer inputs classified
// exactly (secant/tangent/miss via integer cross products) so the expected
// point count is unambiguous; random real inputs vs an independent long-double
// quadratic solve; every point on circle and on line (line_dist). Returned
// order is checked to follow direction a->b. written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/geometry/lineDistance.h"
#include "../../content/geometry/CircleLine.h"

using P = Point<db>;
const db EPS = 1e-8;
bool near(P p, P q) { return (p - q).dist() < EPS; }

int main() {
    { // known values
        auto r = circle_line(P{0, 0}, 1, P{-1, -1}, P{1, 1});
        assert(size(r) == 2 && near(r[0], P{-sqrt(.5), -sqrt(.5)}) && near(r[1], P{sqrt(.5), sqrt(.5)}));
        r = circle_line(P{0, 0}, 1, P{-5, 1}, P{5, 1});
        assert(size(r) == 1 && near(r[0], P{0, 1}));
        r = circle_line(P{4, 4}, 1, P{0, 0}, P{5, 0});
        assert(size(r) == 0);
        r = circle_line(P{2, 3}, 0, P{0, 3}, P{9, 3}); // zero radius on the line
        assert(size(r) == 1 && near(r[0], P{2, 3}));
    }
    mt19937 rng(7);
    auto rnd = [&] (int lo, int hi) { return (int) (rng() % (unsigned) (hi - lo + 1)) + lo; };
    auto soundness = [&] (const vt<P>& r, P c, db rad, P a, P b) {
        assert(size(r) <= 2);
        for (P p : r) assert(abs((p - c).dist() - rad) < EPS && abs(line_dist(a, b, p)) < EPS);
        if (size(r) == 2) assert((r[1] - r[0]).dot(b - a) > 0);
    };
    F0R(it, 200000) { // integer inputs: exact classification
        ll ax = rnd(-5, 5), ay = rnd(-5, 5), bx = rnd(-5, 5), by = rnd(-5, 5), cx = rnd(-5, 5), cy = rnd(-5, 5);
        if (ax == bx && ay == by) continue;
        ll k = rnd(0, 48); // r^2
        P a{(db) ax, (db) ay}, b{(db) bx, (db) by}, c{(db) cx, (db) cy};
        ll cr = (bx - ax) * (cy - ay) - (by - ay) * (cx - ax), d2 = (bx - ax) * (bx - ax) + (by - ay) * (by - ay);
        auto r = circle_line(c, sqrt((db) k), a, b);
        soundness(r, c, sqrt((db) k), a, b);
        if (cr * cr < k * d2) assert(size(r) == 2 && !near(r[0], r[1]));
        else if (cr * cr > k * d2) assert(size(r) == 0);
        else { // tangent: r*r rounds, so h2 may be ~1e-15 and sqrt(h2) ~1e-7; points must still be at the foot
            ll t = (bx - ax) * (cx - ax) + (by - ay) * (cy - ay);
            P foot = a + (b - a) * ((db) t / (db) d2);
            for (P p : r) assert((p - foot).dist() < 1e-6);
        }
    }
    F0R(it, 200000) { // real inputs vs long-double quadratic along the line
        auto rd = [&] { return rnd(-1000, 1000) / 100.0; };
        P a{rd(), rd()}, b{rd(), rd()}, c{rd(), rd()};
        if (a == b) continue;
        db rad = abs(rd());
        auto r = circle_line(c, rad, a, b);
        soundness(r, c, rad, a, b);
        long double dx = b.x - a.x, dy = b.y - a.y, ex = a.x - c.x, ey = a.y - c.y;
        long double A = dx * dx + dy * dy, B = 2 * (dx * ex + dy * ey), C = ex * ex + ey * ey - (long double) rad * rad;
        long double disc = B * B - 4 * A * C;
        if (abs(disc) < 1e-3) continue; // near-tangent: count is legitimately ambiguous
        if (disc < 0) assert(size(r) == 0);
        else {
            assert(size(r) == 2);
            long double t0 = (-B - sqrtl(disc)) / (2 * A), t1 = (-B + sqrtl(disc)) / (2 * A);
            assert(near(r[0], P{(db) (a.x + dx * t0), (db) (a.y + dy * t0)}));
            assert(near(r[1], P{(db) (a.x + dx * t1), (db) (a.y + dy * t1)}));
        }
    }
    cout << "Tests passed!" << endl;
}
