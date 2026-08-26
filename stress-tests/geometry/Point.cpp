// Tests Point.h: compiles with .rotate() instantiated (brace-init fix), and
// checks rotate() against manual trig on random points/angles.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/geometry/Point.h"

int main() {
    mt19937 rng(123);
    auto rnd = [&] { return uniform_real_distribution<db>(-100, 100)(rng); };
    F0R (it, 200000) {
        Point<db> p{rnd(), rnd()};
        db a = uniform_real_distribution<db>(-10, 10)(rng);
        Point<db> r = p.rotate(a);
        db ex = p.x * cos(a) - p.y * sin(a), ey = p.x * sin(a) + p.y * cos(a);
        assert(abs(r.x - ex) < 1e-9 && abs(r.y - ey) < 1e-9);
        assert(abs(r.dist() - p.dist()) < 1e-6); // rotation preserves length
        // rotate by pi/2 matches perp()
        Point<db> q = p.rotate(acos((db) -1) / 2), pp = p.perp();
        assert(abs(q.x - pp.x) < 1e-6 && abs(q.y - pp.y) < 1e-6);
    }
    cout << "Tests passed!" << endl;
}
