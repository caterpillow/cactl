/**
 * Author: Claude
 * Date: 2026-10-08
 * License: CC0
 * Source: de Berg et al., Computational Geometry, ch. 3
 * Description: Triangulates a simple CCW polygon with distinct vertices
 *  (collinear vertices and horizontal edges are fine), $|coord| \le 10^9$.
 *  Returns $n - 2$ CCW index triples. A sweep by $(x, y)$ adds diagonals
 *  at split and merge vertices, giving monotone faces whose convex ears
 *  are clipped in sweep order.
 * Usage: vt<P> poly = {{0,0}, {2,0}, {1,1}, {2,2}, {0,2}};
 *  vt<array<int, 3>> tri = triangulate(poly);
 * Time: O(n \log n)
 * Status: stress-tested
 */
#pragma once

#include "Point.h"

using P = Point<ll>;
vt<array<int, 3>> triangulate(vt<P> p) {
    int n = size(p), m = n;
    p.pb(p[0]);
    vt<vi> E(2, vi(3 * n));
    vi id(3 * n), sm(3 * n, -1), hl(n), mg(n), o(n);
    iota(all(id), 0), iota(all(o), 0);
    auto ln = [&](int x, int y) { E[0][x] = y, E[1][y] = x; };
    F0R(i, n) ln(i, (i + 1) % n);
    auto d = [&](int a, int b) {
        int A = m, B = m + 1;
        for (int x : {a, b})
            x = id[x], id[m] = x, sm[m] = sm[x], sm[x] = m++;
        ln(E[1][a], A), ln(A, B), ln(B, E[0][b]), ln(b, a);
        return A;
    };
    auto cmp = [&](int x, int y) {
        if (p[x] < p[y]) return p[x].cross(p[x + 1], p[y]) > 0;
        return p[y].cross(p[y + 1], p[x]) < 0;
    };
    set<int, decltype(cmp)> T(cmp);
    sort(all(o), [&](int a, int b) { return p[a] < p[b]; });
    for (int v : o) {
        int u = (v + n - 1) % n, w = (v + 1) % n;
        bool L = p[u] < p[v], R = p[w] < p[v];
        if (L) T.erase(u), mg[id[hl[u]]] && d(v, hl[u]);
        auto it = T.insert(v).first;
        hl[v] = v;
        if (L == R ? p[v].cross(p[u], p[w]) > 0 : R) {
            int e = *--it;
            hl[e] = mg[id[hl[e]]] || !(L || R) ? d(v, hl[e]) : v;
            mg[v] = L && R;
        }
        if (R) T.erase(v);
    }
    vt<array<int, 3>> r;
    vi dn(m);
    for (int v : o) for (int u = v; u >= 0; u = sm[u]) {
        F0R(k, 2) for (int w, z; w = E[k][u], z = E[k][w],
          dn[w] && dn[z] && sgn(p[id[u]].cross(p[id[w]], p[id[z]]))
          == 1 - 2 * k; E[k][u] = z, E[!k][z] = u)
            r.pb({id[u], id[k ? z : w], id[k ? w : z]});
        dn[u] = 1;
    }
    return r;
}
