// Check every insertion prefix against batch convex hull and independent
// Jarvis march, including duplicates, collinearity and disappearing vertices.
#include "../utilities/template.h"
#include "../../content/geometry/Point.h"
using P = Point<ll>;
#include "../../content/geometry/ConvexHull.h"
#include "../../content/geometry/IncrementalHull.h"

mt19937_64 rng(20260924);
int sequences = 0, prefixes = 0;

vt<P> jarvis(vt<P> pts) {
    sort(all(pts));
    pts.erase(unique(all(pts)), pts.end());
    if (size(pts) <= 2) return pts;
    vt<P> ans;
    P cur = pts[0];
    do {
        ans.pb(cur);
        P next = cur == pts[0] ? pts[1] : pts[0];
        for (P p : pts) {
            ll turn = cur.cross(next, p);
            if (turn < 0 || (turn == 0 &&
                (p - cur).dist2() > (next - cur).dist2())) next = p;
        }
        cur = next;
        assert(size(ans) <= size(pts));
    } while (!(cur == pts[0]));
    return ans;
}

void check(const IncrementalHull<ll>& hull, const vt<P>& pts) {
    ++prefixes;
    vt<P> got = hull.hull();
    assert(got == convex_hull(pts));
    assert(got == jarvis(pts));
    if (got.empty()) {
        assert(pts.empty());
        return;
    }
    assert(got[0] == *min_element(all(pts)));
    if (size(got) == 1) {
        for (P p : pts) assert(p == got[0]);
    } else if (size(got) == 2) {
        assert(got[0] < got[1]);
        for (P p : pts) {
            assert(got[0].cross(got[1], p) == 0);
            assert((p - got[0]).dot(p - got[1]) <= 0);
        }
    } else {
        F0R(i, size(got)) {
            P a = got[i], b = got[(i + 1) % size(got)];
            assert(a.cross(b, got[(i + 2) % size(got)]) > 0);
            for (P p : pts) assert(a.cross(b, p) >= 0);
        }
    }
}

void test(const vt<P>& pts) {
    ++sequences;
    IncrementalHull<ll> hull;
    vt<P> prefix;
    check(hull, prefix);
    for (P p : pts) {
        hull.add(p);
        prefix.pb(p);
        check(hull, prefix);
    }
}

int main() {
    test({});
    test(vt<P>(40, {3, -7}));
    for (P direction : vt<P>{{1, 0}, {0, 1}, {2, 3}, {-2, 3}}) {
        vt<P> line;
        for (ll t : {0, 1, -1, 4, 2, -3, 4, -7, 9, 0})
            line.pb(P{7, -4} + direction * t);
        test(line);
        reverse(all(line));
        test(line);
    }
    // Old hull vertices become interior points or lie inside a new edge.
    test({{0, 0}, {1, 0}, {0, 1}, {-1, 0}, {0, -1},
          {2, 0}, {0, 2}, {-2, 0}, {0, -2}, {0, 0}});
    test({{0, 0}, {2, 0}, {1, 1}, {2, 2}, {0, 2},
          {1, 0}, {2, 1}, {1, 2}, {0, 1}, {1, 1}, {-2, -2}});
    // Extreme x ties, edge extensions, then all previous points inside.
    test({{0, 0}, {0, 3}, {0, -3}, {0, 1}, {1, 0}, {1, 3},
          {1, -3}, {-1, 0}, {-1, 3}, {-1, -3}, {0, 8},
          {8, 0}, {0, -8}, {-8, 0}});

    vt<P> small = {{0, 0}, {2, 0}, {2, 2}, {0, 2}, {1, 0}, {1, 1}};
    sort(all(small));
    do test(small); while (next_permutation(all(small)));

    const ll bounds[] = {1, 2, 10, 10000, 1000000000};
    F0R(it, 2500) {
        ll bound = bounds[it % 5];
        uniform_int_distribution<ll> coordinate(-bound, bound);
        vt<P> pts;
        int n = rng() % 61;
        F0R(i, n) {
            P p = {coordinate(rng), coordinate(rng)};
            if (it % 13 == 0) p.x = 0;
            if (i && rng() % 5 == 0) p = pts[rng() % size(pts)];
            pts.pb(p);
        }
        test(pts);
        if (it % 11 == 0) {
            sort(all(pts));
            test(pts);
            reverse(all(pts));
            test(pts);
        }
    }

    const ll big = 1000000000;
    vt<P> extremes = {{0, 0}, {-big, -big}, {big, big},
                     {-big, big}, {big, -big}, {0, -big},
                     {0, big}, {-big, 0}, {big, 0}};
    F0R(it, 100) {
        shuffle(all(extremes), rng);
        test(extremes);
    }
    cout << "Tests passed: " << sequences << " sequences, "
         << prefixes << " prefixes.\n";
}
