// Tests AllPointPairs.h: mn/mx vs brute min/max |cross| over all ordered
// triples on distinct points with no three collinear (live variant). The snippet
// needs Point.h's radial operator< uncommented; P here replicates exactly
// that (x, y, perp, cross, half, radial <). written by Claude (audit)
#include "../utilities/template.h"

struct P {
    ll x, y;
    P operator-(P p) const { return {x - p.x, y - p.y}; }
    P perp() const { return {-y, x}; }
    ll cross(P p) const { return x * p.y - y * p.x; }
    ll dot(P p) const { return x * p.x + y * p.y; }
    ll cross(P a, P b) const { return (a - *this).cross(b - *this); }
    int half() const { return y < 0 || (y == 0 && x < 0); }
    bool operator<(P p) const { // the radial comparator from Point.h
        return make_pair(half(), 0ll) < make_pair(p.half(), cross(p));
    }
};

pair<ll, ll> run_snippet(const string& input) {
    istringstream cin(input); // shadows std::cin for the snippet
    #include "../../content/geometry/AllPointPairs.h"
    return {mn, mx};
}

int main() {
    mt19937 rng(2024);
    F0R (it, 200000) {
        int n = 3 + rng() % 6, lim = 4 + rng() % 12;
        vt<P> pts(n);
        string in = to_string(n);
        set<pair<ll, ll>> seen; // distinct points, no three collinear
        bool ok = 1;
        for (P& p : pts) {
            do p = {(ll) (rng() % lim) - lim / 2, (ll) (rng() % lim) - lim / 2};
            while (!seen.insert({p.x, p.y}).second);
            in += " " + to_string(p.x) + " " + to_string(p.y);
        }
        F0R (i, n) F0R (j, i) F0R (k, j) if (pts[i].cross(pts[j], pts[k]) == 0) ok = 0;
        if (!ok) continue;
        auto [mn, mx] = run_snippet(in);
        ll bmn = INF, bmx = -INF;
        F0R (i, n) F0R (j, n) F0R (k, n) if (i != j && j != k && i != k) {
            ll v = abs(pts[i].cross(pts[j], pts[k]));
            bmn = min(bmn, v), bmx = max(bmx, v);
        }
        if (mn != bmn || mx != bmx) {
            cout << "MISMATCH it=" << it << " n=" << n << " got mn=" << mn
                 << " mx=" << mx << " want mn=" << bmn << " mx=" << bmx << "\n"
                 << in << endl;
            return 1;
        }
    }
    cout << "Tests passed!" << endl;
}
