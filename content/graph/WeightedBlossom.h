/**
 * Author: Claude
 * Date: 2026-10-08
 * License: CC0
 * Source: the standard primal-dual Edmonds blossom template (UOJ 81)
 * Description: Maximum weight matching in a general graph (not
 *  necessarily perfect), 1-indexed, adjacency matrix. Edges with
 *  $w \le 0$ and self-loops are never used; parallel edges keep the max.
 * Usage: WeightedBlossom g; g.init(n); g.ae(u, v, w);
 *  ll best = g.solve(); g.mate[v] = partner, 0 if none.
 * Time: O(N^3)
 * Memory: $(2N)^2$ edges of 16 bytes
 * Status: stress-tested
 */
#pragma once

struct WeightedBlossom {
    struct E { int u, v; ll w; };
    int n, nx, tm = 0;
    vt<vt<E>> g;
    vl lab;
    vi mate, sl, st, pa, S, vis;
    vt<vi> fl, ff; // cycle of b; ff[b][x] = child of b with x
    queue<int> q;
    void init(int _n) {
        n = _n; int m = 2 * n + 1;
        g.assign(m, vt<E>(m));
        F0R (u, m) F0R (v, m) g[u][v] = {u, v, 0};
        lab = vl(m), mate = sl = st = pa = S = vis = vi(m);
        fl.assign(m, {}), ff.assign(m, vi(n + 1));
    }
    void ae(int u, int v, ll w) {
        if (u != v) g[u][v].w = g[v][u].w = max(g[u][v].w, w);
    }
    ll d(E e) { return lab[e.u] + lab[e.v] - 2 * g[e.u][e.v].w; }
    void upd(int u, int x) {
        if (!sl[x] || d(g[u][x]) < d(g[sl[x]][x])) sl[x] = u;
    }
    void setsl(int x) {
        sl[x] = 0;
        FOR (u, 1, n + 1)
            if (g[u][x].w > 0 && st[u] != x && !S[st[u]]) upd(u, x);
    }
    void push(int x) {
        if (x <= n) q.push(x);
        else for (int y : fl[x]) push(y);
    }
    void setst(int x, int b) {
        st[x] = b;
        if (x > n) for (int y : fl[x]) setst(y, b);
    }
    int pos(int b, int x) { // even position of x in fl[b]
        int p = int(find(all(fl[b]), x) - begin(fl[b]));
        if (p % 2 == 0) return p;
        reverse(begin(fl[b]) + 1, end(fl[b]));
        return size(fl[b]) - p;
    } // <hash>
    void setm(int u, int v) {
        mate[u] = g[u][v].v;
        if (u <= n) return;
        int x = ff[u][g[u][v].u], p = pos(u, x);
        F0R (i, p) setm(fl[u][i], fl[u][i ^ 1]);
        setm(x, v);
        rotate(begin(fl[u]), begin(fl[u]) + p, end(fl[u]));
    }
    void aug(int u, int v) {
        for (int w; (w = st[mate[u]]); u = st[pa[w]], v = w)
            setm(u, v), setm(w, st[pa[w]]);
        setm(u, v);
    }
    int lca(int u, int v) {
        for (++tm; u || v; swap(u, v)) {
            if (!u) continue;
            if (vis[u] == tm) return u;
            vis[u] = tm, u = st[mate[u]];
            if (u) u = st[pa[u]];
        }
        return 0;
    } // <hash>
    void add(int u, int l, int v) {
        int b = n + 1;
        while (b <= nx && st[b]) b++;
        if (b > nx) nx++;
        lab[b] = S[b] = 0, mate[b] = mate[l], fl[b] = {l};
        for (int x = u, y; x != l; x = st[pa[y]])
            fl[b].pb(x), fl[b].pb(y = st[mate[x]]), push(y);
        reverse(begin(fl[b]) + 1, end(fl[b]));
        for (int x = v, y; x != l; x = st[pa[y]])
            fl[b].pb(x), fl[b].pb(y = st[mate[x]]), push(y);
        setst(b, b);
        FOR (x, 1, nx + 1) g[b][x].w = g[x][b].w = 0;
        FOR (x, 1, n + 1) ff[b][x] = 0;
        for (int c : fl[b]) {
            FOR (x, 1, nx + 1)
                if (!g[b][x].w || d(g[c][x]) < d(g[b][x]))
                    g[b][x] = g[c][x], g[x][b] = g[x][c];
            FOR (x, 1, n + 1) if (ff[c][x]) ff[b][x] = c;
        }
        setsl(b);
    }
    void expand(int b) {
        for (int x : fl[b]) setst(x, x);
        int r = ff[b][g[b][pa[b]].u], p = pos(b, r);
        for (int i = 0; i < p; i += 2) {
            int x = fl[b][i], y = fl[b][i + 1];
            pa[x] = g[y][x].u, S[x] = 1, S[y] = 0;
            sl[x] = 0, setsl(y), push(y);
        }
        S[r] = 1, pa[r] = pa[b];
        FOR (i, p + 1, size(fl[b]))
            S[fl[b][i]] = -1, setsl(fl[b][i]);
        st[b] = 0;
    } // <hash>
    bool found(E e) { // tight edge e from an outer vertex
        int u = st[e.u], v = st[e.v];
        if (S[v] == -1) {
            int w = st[mate[v]];
            pa[v] = e.u, S[v] = 1, sl[v] = sl[w] = S[w] = 0, push(w);
        } else if (!S[v]) {
            int l = lca(u, v);
            if (!l) return aug(u, v), aug(v, u), 1;
            add(u, l, v);
        }
        return 0;
    }
    bool phase() { // 1 if the matching grew
        q = {};
        FOR (x, 1, nx + 1) S[x] = -1, sl[x] = 0;
        FOR (x, 1, nx + 1)
            if (st[x] == x && !mate[x]) pa[x] = S[x] = 0, push(x);
        if (q.empty()) return 0;
        while (1) {
            for (; size(q); q.pop()) {
                int u = q.front();
                if (S[st[u]] == 1) continue;
                FOR (v, 1, n + 1) {
                    if (g[u][v].w <= 0 || st[u] == st[v]) continue;
                    if (d(g[u][v])) upd(u, st[v]);
                    else if (found(g[u][v])) return 1;
                }
            }
            ll t = INF;
            FOR (b, n + 1, nx + 1)
                if (st[b] == b && S[b] == 1) t = min(t, lab[b] / 2);
            FOR (x, 1, nx + 1) if (st[x] == x && sl[x] && S[x] != 1)
                t = min(t, d(g[sl[x]][x]) / (S[x] ? 1 : 2));
            FOR (u, 1, n + 1) {
                if (!S[st[u]]) {
                    if (lab[u] <= t) return 0;
                    lab[u] -= t;
                } else if (S[st[u]] == 1) lab[u] += t;
            }
            FOR (b, n + 1, nx + 1) if (st[b] == b && S[b] >= 0)
                lab[b] += S[b] ? -2 * t : 2 * t;
            q = {};
            FOR (x, 1, nx + 1)
                if (st[x] == x && sl[x] && st[sl[x]] != x &&
                        !d(g[sl[x]][x]) && found(g[sl[x]][x])) return 1;
            FOR (b, n + 1, nx + 1)
                if (st[b] == b && S[b] == 1 && !lab[b]) expand(b);
        }
    } // <hash>
    ll solve() {
        int m = 2 * n + 1;
        mate = st = vi(m), nx = n;
        F0R (u, n + 1) st[u] = u, fl[u].clear();
        ll mx = 0;
        FOR (u, 1, n + 1) FOR (v, 1, n + 1)
            ff[u][v] = u == v ? u : 0, mx = max(mx, g[u][v].w);
        FOR (u, 1, n + 1) lab[u] = mx;
        while (phase());
        ll r = 0;
        FOR (u, 1, n + 1) if (mate[u] > u) r += g[u][mate[u]].w;
        return r;
    } // <hash>
};
