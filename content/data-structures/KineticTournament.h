/**
 * Author: caterpillow
 * Date: 2026-09-09
 * License: CC0
 * Source: https://codeforces.com/blog/entry/82094
 * Description: \texttt{seg[1]} is the point minimising $p \cdot t$ while
 * the direction $t$ sweeps CCW from t0 to tn, less than a half turn
 * (asserted). \texttt{heaten} advances $t$ (never back), \texttt{upd}
 * moves a point, fine mid-sweep. \texttt{mt[i]} is the first direction at
 * which node $i$'s winner can change; a subtree is rebuilt only once $t$
 * reaches it. For argmax flip the \texttt{<} in combine. Needs Point's
 * radial \texttt{<} and a sweep inside angles $[0, 180)$: elsewhere
 * certificates wrap past $0$ and the tree recomputes needlessly (still
 * correct). Lines $ax+b$ at increasing $x$: point $(-a,b)$, t0 $(B,1)$,
 * heaten $(-x,1)$, tn $(-B,1)$. Safe for $|coord|, |t| \le 10^9$ (a cross
 * of two certificates reaches $8 \cdot 10^{18}$); Point<\_\_int128> beyond.
 * Time: O(\log n) per upd. A node's winner changes at most (subtree
 * size) times per sweep, so O(n \log n) rebuilds, each walking a root
 * path: O(n \log^2 n) per sweep, i.e. O(\log^2 n) amortized per heaten.
 * Status: stress-tested
 */
#pragma once

#include "../geometry/Point.h"

using P = Point<ll>; // radial <; __int128 if cross overflows
struct KineticTournament {
    int n; vt<P> seg, mt; P t, tn;
    KineticTournament(vt<P> a, P t0, P tn)
        : n(size(a)), seg(2 * n), mt(2 * n, tn), t(t0), tn(tn) {
        assert(t0.cross(tn) > 0); // CCW turn < 180
        copy(all(a), n + begin(seg));
        ROF (i, 1, n) combine(i);
    }
    void combine(int i) { // rebuild i from its children
        P p = seg[2 * i], q = seg[2 * i + 1];
        if (q.dot(t) < p.dot(t)) swap(p, q); // p = argmin
        seg[i] = p, mt[i] = min(mt[2 * i], mt[2 * i + 1]);
        P e = (q - p).perp(); // where p, q tie; ahead of t
        if (!(tn < e)) mt[i] = min(mt[i], e);
    }
    void pull(int i) { // recurse into expired subtrees
        if (i >= n || t < mt[i]) return;
        pull(2 * i), pull(2 * i + 1), combine(i);
    }
    void upd(int i, P p) { for (seg[i+=n] = p; i /= 2; combine(i)); }
    void heaten(P nt) { assert(!(nt < t)); t = nt, pull(1); }
};
