/**
 * Author: caterpillow
 * Date: 2026-09-06
 * License: CC0
 * Description: Holds points $p_i$ and reports $\max_i p_i \cdot t$ as the
 * query direction $t$ sweeps monotonically CCW through less than a half
 * turn; \texttt{upd} moves a point. Each \texttt{heaten} direction must be
 * $\ge$ the last under the angular \texttt{<} (a.cross(b) > 0), and all
 * of them plus t0/END must lie in one open half-plane; pick t0/END just
 * before/after the sweep (also outside the data, so no sentinel clashes).
 * Lines $ax+b$ at increasing $x$: point $(a,-b)$, t0 $(-B,-1)$,
 * heaten $(x,-1)$, END $(B,-1)$. Safe while $|coord|,|t| \le 10^9$.
 * Time: O((n + f) \log n) over a sweep, f = certificate breaks; f is
 * O(n \log n), so O(n \log^2 n) total.
 * Status: stress-tested
 */
#pragma once

struct P {
    ll x, y;
    P operator-(P p) const { return {x - p.x, y - p.y}; }
    ll dot(P p) const { return x * p.x + y * p.y; }
    ll cross(P p) const { return x * p.y - y * p.x; }
    P perp() const { return {-y, x}; }
    bool operator<(P p) const { return cross(p) > 0; } // p CCW
};

struct KineticTournament {
    int n; vt<P> win, cert; P t, END; // cert[v]: next swap dir
    KineticTournament(vt<P> a, P t0, P end)
        : n(size(a)), win(4*n), cert(4*n), t(t0), END(end) {
        build(1, 0, n, a);
    }
    void pull(int v) {
        P p = win[2*v], q = win[2*v+1];
        win[v] = q.dot(t) > p.dot(t) ? q : p;
        cert[v] = min(cert[2*v], cert[2*v+1]);
        P e = (p - q).perp(); // future dir where p, q tie
        if (e < t) e = {-e.x, -e.y};
        if (e < END) cert[v] = min(cert[v], e);
    }
    void build(int v, int l, int r, vt<P> &a) {
        if (r - l == 1) { win[v]=a[l], cert[v]=END; return; }
        int m = (l + r) / 2;
        build(2*v, l, m, a), build(2*v+1, m, r, a), pull(v);
    }
    void heat(int v, int l, int r) { // recompute expired certs
        if (t < cert[v] || r - l == 1) return;
        int m = (l + r) / 2;
        heat(2*v, l, m), heat(2*v+1, m, r), pull(v);
    }
    void upd(int v, int l, int r, int i, P p) {
        if (r - l == 1) { win[v] = p; return; }
        int m = (l + r) / 2;
        i < m ? upd(2*v, l, m, i, p) : upd(2*v+1, m, r, i, p);
        pull(v);
    }
    void heaten(P nt) { assert(!(nt<t)); t=nt, heat(1,0,n); }
    void upd(int i, P p) { upd(1, 0, n, i, p); }
    ll query() { return win[1].dot(t); } // argmax = win[1]
};
