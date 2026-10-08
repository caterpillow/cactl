/**
 * Author: Claude
 * Date: 2026-10-07
 * License: CC0
 * Source: Gabow, The Weighted Matching Approach to Maximum
 *  Cardinality Matching (2017), via Min\_25's implementation
 * Description: Maximum matching in a general graph, 1-indexed;
 *  same interface as Blossom. Self-loops and multi-edges are fine.
 * Usage: GabowMatching g; g.init(n); g.ae(u, v); // once per edge
 *  g.solve() = matching size; g.mate[v] = partner, 0 if none.
 * Time: O(\sqrt{N} M \log N); $10^6$ vertices, $2 \cdot 10^6$ random edges: 1s.
 *  Odd cycles hanging off a long cycle: 2s at $3 \cdot 10^5$.
 * Status: stress-tested
 */
#pragma once
struct GabowMatching {
    int n, nh, tc, ta, oid, qh, lgs;
    vt<vi> g, bl;
    vt<vt<pi>> e;
    vi mate, pot, lab, par, q, st;
    vt<pi> lk, lg;
    void init(int _n) { n = _n, g.assign(n + 1, {}), bl = g; }
    void ae(int u, int v) { g[u].pb(v), g[v].pb(u); }
    int find(int u) {
        return par[u] == u ? u : par[u] = find(par[u]);
    }
    void join(int x, int y) {
        if (!lab[y]) {
            int z = mate[y];
            lab[y] = -1, pot[y] = tc, lk[z] = {x, y};
            lab[z] = lab[x], pot[z] = tc + 1, q.pb(z);
            return;
        }
        int bx = find(x), by = find(y), h = -2 - size(lg), l;
        lab[mate[bx]] = lab[mate[by]] = h;
        while (1) {
            if (mate[by]) swap(bx, by);
            bx = l = find(lk[bx].f);
            if (lab[mate[bx]] == h) break;
            lab[mate[bx]] = h;
        }
        for (int b : {par[x], par[y]})
            for (; b != l; b = par[lk[b].f]) {
                int m = mate[b];
                lk[m] = {x, y}, lab[m] = lab[x], q.pb(m);
                pot[m] = 2 * tc + 1 - pot[m];
                par[b] = par[m] = l, lg.pb({b, l}), lg.pb({m, l});
            }
    } // <hash>
    bool path() {
        while (qh < size(q)) {
            int x = q[qh++], lx = lab[x], px = pot[x];
            int bx = find(x);
            for (int y : g[x]) {
                if (lab[y] < 0) continue;
                int t = lab[y] ? (px + pot[y]) >> 1 : px + 1;
                if (lab[y] > 0 && lx != lab[y]) {
                    if (t == tc) return 1;
                    ta = min(ta, t);
                } else if (!lab[y] || bx != find(y)) {
                    if (t == tc) join(x, y), bx = find(x);
                    else if (t <= nh) e[t].pb({x, y});
                }
            }
        }
        return 0;
    } // <hash>
    bool dual() { // 1 if the matching is maximum
        int lim = min(nh + 1, ta);
        for (++tc; lgs = size(lg), tc < lim; ++tc) {
            bool up = 0;
            for (auto [x, y] : e[tc]) {
                if (lab[y] > 0) {
                    if (pot[x] + pot[y] != 2 * tc) continue;
                    if (find(x) == find(y)) continue;
                    if (lab[x] != lab[y]) return ta = tc, 0;
                } else if (lab[y]) continue;
                join(x, y), up = 1;
            }
            if (up) return 0;
        }
        return tc > nh;
    } // <hash>
    void rematch(int v, int w) {
        int t = mate[v]; mate[v] = w;
        if (mate[t] != v) return;
        auto [x, y] = lk[v];
        if (y == find(y)) mate[t] = x, rematch(x, t);
        else rematch(x, y), rematch(y, x);
    }
    bool dfs(int x, int bx) {
        int px = pot[x], lx = lab[bx];
        for (int y : g[x]) {
            if (px + pot[y]) continue;
            int by = find(y), ly = lab[by];
            if (ly > 0) {
                if (lx >= ly) continue;
                int b = size(st);
                for (int v = by; v != bx; v = find(lk[v].f)) {
                    int w = find(mate[v]);
                    st.pb(w), lk[w] = {x, y}, par[v] = par[w] = bx;
                }
                for (int i = size(st); i-- > b; st.pop_back())
                    for (int u : bl[st[i]])
                        if (dfs(u, bx)) return 1;
            } else if (!ly) {
                lab[by] = -1;
                int z = mate[by];
                if (!z) return rematch(x, y), rematch(y, x), 1;
                int bz = find(z);
                lk[bz] = {x, y}, lab[bz] = oid++;
                for (int u : bl[bz]) if (dfs(u, bz)) return 1;
            }
        }
        return 0;
    } // <hash>
    int init_match() { // greedy, degree-1 vertices first
        int r = 0;
        vi d(n + 1), s, t;
        auto take = [&](int u) {
            int b = 0;
            for (int v : g[u])
                if (v != u && !mate[v] && (!b || d[v] < d[b])) b = v;
            if (!b) return;
            mate[u] = b, mate[b] = u, r++;
            for (int w : {u, b})
                for (int x : g[w])
                    if (!mate[x]) (--d[x] == 1 ? s : t).pb(x);
        };
        FOR (u, 1, n + 1) if ((d[u] = size(g[u])) == 1) s.pb(u);
        FOR (u, 1, n + 1) for (t.pb(u); size(s) + size(t); ) {
            auto& z = size(s) ? s : t;
            int x = z.back(); z.pop_back();
            if (!mate[x]) take(x);
        }
        return r;
    }
    int solve() {
        nh = n / 2, mate = pot = lab = par = vi(n + 1);
        lk.assign(n + 1, {}), e.assign(n + 1, {});
        int r = init_match(); // optional, or r = 0
        while (2 * r + 1 < n) {
            tc = qh = lgs = 0, ta = inf, oid = 1;
            q.clear(), lg.clear(), st.clear();
            FOR (u, 0, n + 1) {
                pot[u] = 1, par[u] = u, bl[u].clear(), e[u].clear();
                lab[u] = u && !mate[u] ? (q.pb(u), u) : 0;
            }
            while (!path()) {
                if (dual()) return r;
                if (tc == ta) break;
            }
            FOR (u, 1, n + 1) {
                if (lab[u] > 0) pot[u] -= tc;
                else if (lab[u] < 0) pot[u] = 1 + tc - pot[u];
                lab[u] = 0, par[u] = u;
            }
            F0R (i, lgs) par[lg[i].f] = lg[i].s;
            FOR (u, 1, n + 1) bl[find(u)].pb(u);
            FOR (u, 1, n + 1) if (!mate[u] && !lab[par[u]]) {
                int b = par[u];
                lab[b] = oid++;
                for (int v : bl[b]) if (dfs(v, b)) { r++; break; }
            }
        }
        return r;
    } // <hash>
};
