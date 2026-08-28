// Tests LineProjectionReflection.h (proj): projection vs. the direct formula
// a + v * ((p-a).v / v.v), on-line + perpendicular + nearest-point checks,
// reflection midpoint/isometry, and exact Point<ll> known values.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/geometry/LineProjectionReflection.h"
#include "../../content/geometry/lineDistance.h"

int main() {
    mt19937 rng(2024);
    using P = Point<db>;
    F0R (it, 200000) {
        auto r = [&] { return (db) (int) (rng() % 11) - 5; };
        P a{r(), r()}, b{r(), r()}, p{r(), r()};
        if (a == b) continue;
        P v = b - a;
        P q = proj(a, b, p), w = proj(a, b, p, true);
        P brute = a + v * ((p - a).dot(v) / v.dist2());
        assert((q - brute).dist() < 1e-9);
        assert(abs(line_dist(a, b, q)) < 1e-9);
        assert(abs((p - q).dot(v)) < 1e-9);
        assert(abs((p - q).dist() - abs(line_dist(a, b, p))) < 1e-9);
        assert(((p + w) / 2 - q).dist() < 1e-9); // q is midpoint of p, refl
        assert(abs((w - a).dist() - (p - a).dist()) < 1e-9);
        assert(abs(line_dist(a, b, w) + line_dist(a, b, p)) < 1e-9);
    }
    // integer points: build p = a + v*t + perp(v)*u so results are exact
    using L = Point<ll>;
    F0R (it, 100000) {
        auto r = [&] { return (ll) (rng() % 21) - 10; };
        L a{r(), r()}, v{r(), r()};
        if (v == L{0, 0}) continue;
        ll t = r(), u = r();
        L p = a + v * t + v.perp() * u;
        assert(proj(a, a + v, p) == a + v * t);
        assert(proj(a, a + v, p, true) == a + v * t - v.perp() * u);
    }
    cout << "Tests passed!" << endl;
}
