/**
 * Author: caterpillow
 * Date: 2025-08-26
 * Source: me
 * Description: KD-tree (2d)
 * Status: uhhh idk
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
        // ll res = 0;
        // F0R (i, 2) {
        //     ll tmp = (p[i] < lo[i] ? lo[i] - p[i] : 0) + (hi[i] < p[i] ? p[i] - hi[i] : 0);
        //     res += tmp * tmp;
        // }
        // return res;
    }

    template<class ptr>
    Node (ptr l, ptr r, int d) : lc(0), rc(0) {
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

    void search(P p, ll &best) {
        if (lc) { // rc will also exist
            ll dl = lc->dist2(p), dr = rc->dist2(p);
            if (dl > dr) swap(lc, rc), dr = dl;
            lc->search(p, best);
            if (dr < best) rc->search(p, best);
        } else best = min(best, dist2(p, lo));
    } 

    // fill pq with k infinities for nearest k points
    void search(P p, priority_queue<ll> &pq) {
        if (lc) {
            ll dl = lc->dist2(p), dr = rc->dist2(p);
            if (dl > dr) swap(lc, rc), dr = dl;
            lc->search(p, pq);
            if (dr < pq.top()) rc->search(p, pq);
        } else pq.push(dist2(p, lo)), pq.pop();
    }
};