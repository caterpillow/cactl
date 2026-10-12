/**
 * Author: caterpillow
 * Date: 2025-08-26
 * Source: me
 * Description: KD-tree (2d), squared distances. Coordinates in $[-10^9, 10^9]$.
 * Usage: vt<P> v; // P = array<int, 2>
 *  Node *t = new Node(all(v), 0); // reorders v
 *  ll b = LLONG_MAX; t->search(q, b); // b = nearest dist2
 * Status: stress-tested
 */
#pragma once

#include "Point.h"

using P = array<int, 2>;
struct Node {
    #define _sq(x) (x) * (x)
    P lo, hi;
    struct Node *lc, *rc;

    ll dist2(const P &a, const P &b) const {
        return 1ll * _sq(a[0] - b[0]) + 1ll * _sq(a[1] - b[1]);
    }

    ll dist2(P &p) {
		#define _loc(i) (p[i] < lo[i] ? lo[i] : (p[i] > hi[i] ? hi[i] : p[i]))
        return dist2(p, {_loc(0), _loc(1)});
    }

    template<class ptr>
    Node (ptr l, ptr r, int d) : lc(0), rc(0) {
        assert(l != r);
        lo = {inf, inf}, hi = {-inf, -inf};
        for (ptr p = l; p < r; p++) {
            F0R (i, 2) lo[i] = min(lo[i], (*p)[i]), hi[i] = max(hi[i], (*p)[i]);
        }
        if (r - l == 1) return;
        ptr m = l + (r - l) / 2;
        nth_element(l, m, r, [&] (auto a, auto b) { return a[d] < b[d]; });
        lc = new Node(l, m, d ^ 1);
        rc = new Node(m, r, d ^ 1);
    }

    // nearest neighbour: init best = LLONG_MAX; 0 if p in set
    void search(P p, ll &best) {
        if (lc) { // rc will also exist
            ll dl = lc->dist2(p), dr = rc->dist2(p);
            if (dl > dr) swap(lc, rc), dr = dl;
            lc->search(p, best);
            if (dr < best) rc->search(p, best);
        } else best = min(best, dist2(p, lo));
    }

    // push k LLONG_MAX; top() = kth nearest
    void search(P p, priority_queue<ll> &pq) {
        if (lc) {
            ll dl = lc->dist2(p), dr = rc->dist2(p);
            if (dl > dr) swap(lc, rc), dr = dl;
            lc->search(p, pq);
            if (dr < pq.top()) rc->search(p, pq);
        } else pq.push(dist2(p, lo)), pq.pop();
    }
};
// Degenerate input (many equal x or y): rotate all points and
// queries by p -> {3x - 4y, 4x + 3y}. M^T M = 25 I, so dist
// scales by 5 and dist2 by exactly 25 (divide b by 25). Needs
// |x|, |y| <= 1.4e8: rotated coords <= 7 max|x| must fit inf.
// ~8x fewer nodes visited on equal-x input, same on random.
// Other 2D: {a, -b; b, a}, a^2 + b^2 = s^2, scale s:
// (5, 12, 13), (20, 21, 29).
// 3D, scale 7: rows (3, -2, 6), (6, 3, -2), (-2, 6, 3)
