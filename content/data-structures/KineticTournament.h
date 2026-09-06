/**
 * Author: caterpillow
 * Date: 2026-09-07
 * License: CC0
 * Description: Points $p_i$; \texttt{seg[1]} is $\arg\min_i p_i\cdot t$ as
 * the direction $t$ sweeps CCW from t0 to tn, strictly under a half turn
 * (\texttt{assert(t0.cross(tn) > 0)}). \texttt{upd} fills or moves a point
 * (fine mid-sweep); \texttt{heaten} advances $t$, which must not go back
 * under the angular \texttt{<}. \texttt{pad} is the value of unused leaves:
 * a point whose dot beats every real one over the whole window (so never
 * the argmin), i.e. far along the middle of the sweep, e.g. $\{B,B\}$ if
 * the sweep stays in $x+y>0$. For argmax, flip the \texttt{<} in combine
 * and put pad on the far side. Safe while directions, $|coord| \le 10^9$.
 * Time: O((n + f) \log n) per sweep, f = O(n \log n) melt events
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
    int n; vt<P> seg, mt; P t, tn;
    KineticTournament(int cnt, P t0, P tn, P pad)
        : t(t0), tn(tn) {
        assert(t0.cross(tn) > 0); // CCW turn < 180
        for (n = 1; n < cnt; n *= 2) {}
        seg.assign(2 * n, pad), mt.assign(2 * n, t0);
        FOR (i, n, 2 * n) mt[i] = tn; // leaves never melt
        ROF (i, 1, n) combine(i);
    }
    void combine(int i) { // rebuild i from its children
        P p = seg[2*i], q = seg[2*i+1];
        if (q.dot(t) < p.dot(t)) swap(p, q); // p = argmin
        seg[i] = p, mt[i] = min(mt[2*i], mt[2*i+1]);
        P e = (q - p).perp(); // future dir where p, q swap
        if (e < t) e = {-e.x, -e.y};
        if (e < tn) mt[i] = min(mt[i], e);
    }
    void pull(int i) { // recurse into expired subtrees
        if (i >= n || t < mt[i]) return;
        pull(2*i), pull(2*i+1), combine(i);
    }
    void upd(int i, P p) { seg[i+=n]=p; while (i/=2) combine(i); }
    void heaten(P nt) { assert(!(nt < t)); t = nt, pull(1); }
};
