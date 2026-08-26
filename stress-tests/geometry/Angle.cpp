// Tests Angle.h (ll coords): sort order matches atan2 order for coords ~1e9,
// t90/t180/t360 consistency, angleDiff vs real angle difference (run with
// -fsanitize=undefined to catch overflow). written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/geometry/Angle.h"

const double PI = acos(-1.0);
double norm(Angle a) { // angle in [0, 2pi) + full turns
    double t = atan2((double) a.y, (double) a.x);
    if (t < 0) t += 2 * PI;
    return t + 2 * PI * a.t;
}

int main() {
    mt19937_64 rng(7);
    auto C = [&] { ll v = 0; while (!v) v = (ll) (rng() % 2000000001) - 1000000000; return v; };
    F0R (it, 200) {
        vt<Angle> v;
        F0R (i, 1000) v.pb(Angle(C(), C(), (int) (rng() % 3)));
        sort(all(v), [] (Angle a, Angle b) { return a < b; });
        F0R (i, size(v) - 1) {
            assert(!(v[i + 1] < v[i]));               // exact comparator consistency
            assert(norm(v[i + 1]) - norm(v[i]) > -1e-7); // matches atan2 order
        }
    }
    F0R (it, 200000) { // angleDiff & rotations sanity on large coords
        Angle a(C(), C(), (int) (rng() % 3)), b(C(), C(), (int) (rng() % 3));
        Angle d = angleDiff(a, b);
        double got = norm(d), want = norm(b) - norm(a);
        assert(abs(got - want) < 1e-6 * max(1.0, abs(want)));
        assert(abs(norm(a.t180()) - (norm(a) + PI)) < 1e-7);
        assert(abs(norm(a.t90()) - (norm(a) + PI / 2)) < 1e-7);
        assert(abs(norm(a.t360()) - (norm(a) + 2 * PI)) < 1e-7);
    }
    cout << "Tests passed!" << endl;
}
