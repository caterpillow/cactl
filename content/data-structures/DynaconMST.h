/**
 * Author: caterpillow
 * Date: 2026-08-28
 * Description: Offline dynamic connectivity via IncrementalMST: an edge
 * gets weight $-$(deletion time), so at time $t$ exactly the edges with
 * weight $\le -t-1$ are alive and deletions cost nothing. Events are
 * processed in time order; simple graph (no parallel edges).
 * Usage: ev[t] = {0 add / 1 del / 2 query, u, v}; returns query answers
 * Time: O((N + Q) \log N) expected
 * Status: stress-tested
 */
#pragma once

#include "IncrementalMST.h"

vt<bool> dynacon(int n, vt<array<int, 3>> &ev) {
    int q = size(ev);
    map<pi, int> at; vi del(q, q); vt<bool> res;
    F0R (t, q) { // pass 1: deletion time of each added edge
        auto [ty, u, v] = ev[t];
        if (u > v) swap(u, v);
        if (ty == 0) at[{u, v}] = t;
        if (ty == 1) del[at[{u, v}]] = t, at.erase({u, v});
    }
    DSU dsu(n);
    F0R (t, q) {
        auto [ty, u, v] = ev[t];
        if (ty == 0) dsu.merge(u, v, {-del[t], t});
        if (ty == 2)
            res.pb(dsu.find(u, -t - 1) == dsu.find(v, -t - 1));
    }
    return res;
}
