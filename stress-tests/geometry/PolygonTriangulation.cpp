// Tests PolygonTriangulation.h: n-2 CCW non-degenerate triangles whose areas
// sum to the polygon's, every polygon edge used once and every diagonal twice,
// diagonals strictly inside (n small). Random simple polygons on tiny grids
// (collinear points, equal y), stars, combs and polyominoes with every lattice
// point kept, each also transposed. written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/geometry/PolygonTriangulation.h"

mt19937 rng(7);
ll cr(P a, P b, P c) { return (b - a).cross(c - a); }
ll area2(const vt<P>& p) {
    ll s = 0;
    F0R(i, size(p)) s += p[i].cross(p[(i + 1) % size(p)]);
    return s;
}
bool onseg(P a, P b, P p) {
    return !cr(a, b, p) && min(a.x, b.x) <= p.x && p.x <= max(a.x, b.x) &&
        min(a.y, b.y) <= p.y && p.y <= max(a.y, b.y);
}
bool inter(P a, P b, P c, P d) { // closed segments
    ll d1 = cr(a, b, c), d2 = cr(a, b, d), d3 = cr(c, d, a), d4 = cr(c, d, b);
    if (d1 * d2 < 0 && d3 * d4 < 0) return 1;
    return onseg(a, b, c) || onseg(a, b, d) || onseg(c, d, a) || onseg(c, d, b);
}
bool simple(const vt<P>& p) {
    int n = size(p);
    F0R(i, n) F0R(j, i) {
        int i2 = (i + 1) % n, j2 = (j + 1) % n;
        if (i2 == j || j2 == i) {
            int s = i2 == j ? j : i, a = s == j ? i : i2, b = s == j ? j2 : j;
            if (!cr(p[a], p[s], p[b]) && (p[a] - p[s]).dot(p[b] - p[s]) > 0) return 0;
        } else if (inter(p[i], p[i2], p[j], p[j2])) return 0;
    }
    return area2(p) != 0;
}
bool inside(const vt<P>& p, P q) { // q in doubled coordinates, strictly inside
    int c = 0, n = size(p);
    F0R(i, n) {
        P a = p[i] * 2, b = p[(i + 1) % n] * 2;
        if (onseg(a, b, q)) return 0;
        if ((a.y > q.y) != (b.y > q.y) &&
            (b.y > a.y) == (cr(a, b, q) > 0)) c ^= 1;
    }
    return c;
}
void check(vt<P> p, bool full) {
    if (area2(p) < 0) reverse(all(p));
    int n = size(p);
    auto T = triangulate(p);
    assert(size(T) == n - 2);
    ll tot = 0; map<pi, int> use;
    for (auto t : T) {
        ll a = cr(p[t[0]], p[t[1]], p[t[2]]);
        assert(a > 0); tot += a;
        F0R(k, 3) { int u = t[k], v = t[(k + 1) % 3]; use[{min(u, v), max(u, v)}]++; }
    }
    assert(tot == area2(p));
    for (auto [e, c] : use) {
        auto [u, v] = e;
        bool edge = v == u + 1 || (!u && v == n - 1);
        assert(c == (edge ? 1 : 2));
        if (edge || !full) continue;
        F0R(i, n) {
            int j = (i + 1) % n;
            assert(i == u || i == v || !onseg(p[u], p[v], p[i]));
            if (i != u && i != v && j != u && j != v) assert(!inter(p[u], p[v], p[i], p[j]));
        }
        assert(inside(p, p[u] + p[v]));
    }
    F0R(i, n) assert(use.count({min(i, (i + 1) % n), max(i, (i + 1) % n)}));
}
vt<P> randomPoly(int n, int C) { // 2-opt untangling of random grid points
    while (1) {
        set<P> s;
        while (size(s) < n) s.insert(P{ll(rng() % (C + 1)), ll(rng() % (C + 1))});
        vt<P> p(all(s)); shuffle(all(p), rng);
        bool any = 1;
        for (int r = 0; any && r < 300; r++) { // 2-opt can cycle
            any = 0;
            F0R(i, n) FOR(j, i + 2, n) if (i || j != n - 1)
                if (inter(p[i], p[i + 1], p[j], p[(j + 1) % n]))
                    reverse(p.begin() + i + 1, p.begin() + j + 1), any = 1;
        }
        if (!any && simple(p)) return p;
    }
}
vt<P> polyomino(int cells, int C) { // boundary with every lattice point kept
    while (1) {
        set<pi> s{{0, 0}}; vt<pi> v{{0, 0}};
        while (size(s) < cells) {
            auto [x, y] = v[rng() % size(v)]; int d = rng() % 4;
            int nx = x + (d == 0) - (d == 1), ny = y + (d == 2) - (d == 3);
            if (abs(nx) <= C && abs(ny) <= C && s.insert({nx, ny}).s) v.pb({nx, ny});
        }
        map<P, vt<P>> out;
        for (auto [x, y] : s) {
            P c[4] = {{x, y}, {x + 1, y}, {x + 1, y + 1}, {x, y + 1}};
            F0R(k, 4) {
                P a = c[k], b = c[(k + 1) % 4]; auto& r = out[b];
                auto it = find(all(r), a);
                if (it != r.end()) r.erase(it); else out[a].pb(b);
            }
        }
        int e = 0; P st{};
        bool ok = 1;
        for (auto& [a, r] : out) { if (size(r) > 1) ok = 0; if (size(r)) e++, st = a; }
        if (!ok) continue;
        vt<P> p; P cur = st;
        do p.pb(cur), cur = out[cur][0]; while (cur != st && size(p) <= e);
        if (size(p) == e && simple(p)) return p;
    }
}
int main() {
    F0R(it, 20000) {
        vt<P> p = randomPoly(3 + rng() % 12, 3 + rng() % 5); // >= 16 points
        check(p, 1);
        for (auto& q : p) swap(q.x, q.y);
        check(p, 1);
    }
    F0R(it, 2000) {
        vt<P> p = polyomino(2 + rng() % 40, 6);
        check(p, 1);
        for (auto& q : p) swap(q.x, q.y);
        check(p, 1);
    }
    FOR(k, 1, 60) { // combs: every tooth makes split/merge vertices
        vt<P> p;
        F0R(i, k) p.pb({4 * i, 0}), p.pb({4 * i, 9}), p.pb({4 * i + 2, 9}), p.pb({4 * i + 2, 0});
        p.pb({4 * k - 2, -9}), p.pb({0, -9});
        check(p, 1);
        for (auto& q : p) swap(q.x, q.y);
        check(p, 1);
    }
    vt<P> big; // star with 2e5 vertices at |coord| <= 1e9
    F0R(i, 200000) {
        db a = 2 * M_PI * i / 200000, r = 5e8 + rng() % 400000000;
        big.pb({llround(r * cos(a)), llround(r * sin(a))});
    }
    check(big, 0);
    cout << "Tests passed!" << endl;
}
