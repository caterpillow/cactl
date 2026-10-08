/**
 * Author: caterpillow
 * Date: 2025-09-13
 * License: CC0
 * Source: folklore
 * Description: DSU with rollbacks.
 * Time: $O(\log(N))$
 * Status: good
 */
#pragma once

struct DSU {
    vi e, stk;
    vt<pi> upds;
    DSU(int n) : e(n, -1) {}
    int find(int x) { return e[x] < 0 ? x : find(e[x]); }
    int unite(int x, int y) {
        x = find(x), y = find(y);
        if (x == y) return 0;
        if (e[x] < e[y]) swap(x, y);
        upds.pb({x, e[x]});
        e[y] += e[x];
        e[x] = y;
        return 1;
    }
    void push() { stk.pb(size(upds)); }
    void pop() {
        ROF (i, stk.back(), size(upds)) {
            auto [x, sz] = upds[i];
            e[e[x]] -= sz, e[x] = sz;
        }
        upds.resize(stk.back());
        stk.pop_back();
    }
};
