// Tests lineIntersection.h (line_inter): returned point lies on both lines,
// parallel vs coincident classification on a small grid.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/geometry/lineIntersection.h"
#include "../../content/geometry/lineDistance.h"

int main() {
    mt19937 rng(1337);
    const int GRID = 10;
    F0R (t, 200000) {
        auto r = [&] { return (db) (int) (rng() % GRID); };
        Point<db> a{r(), r()}, b{r(), r()}, c{r(), r()}, d{r(), r()};
        if (a == b || c == d) continue; // degenerate segments aren't lines
        auto res = line_inter(a, b, c, d);
        if (res.f == 1) {
            assert(abs(line_dist(a, b, res.s)) < 1e-8);
            assert(abs(line_dist(c, d, res.s)) < 1e-8);
        } else {
            assert((b - a).cross(d - c) == 0); // parallel
            // -1 iff same line, 0 otherwise; point is P{} = (0,0)
            assert(res.f == -(a.cross(b, c) == 0 ? 1 : 0));
            assert(res.s == (Point<db>{}));
        }
    }
    cout << "Tests passed!" << endl;
}
