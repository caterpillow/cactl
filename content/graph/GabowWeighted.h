/**
 * Author: Claude
 * Date: 2026-10-08
 * License: CC0
 * Source: Gabow, Data Structures for Weighted Matching and Extensions
 *  to b-matching and f-factors (2016), via Min\_25's implementation
 * Description: Maximum weight matching in a general graph (not
 *  necessarily perfect), 1-indexed. Weights $|w| \le 10^9$; negative
 *  edges and self-loops are never used.
 * Usage: GabowWeighted g; g.init(n); g.ae(u, v, w);
 *  ll best = g.solve(); g.mate[v] = partner, 0 if none.
 * Time: O(NM \log N). Include \texttt{bits/extc++.h} before the template.
 * Status: stress-tested
 */
#pragma once

#include <bits/extc++.h> /** keep-include */

struct GabowWeighted {
    using A = array<ll, 4>; // {time, from, to, id}
    using PQ = __gnu_pbds::priority_queue<A, greater<A>>;
    struct Heaps { // item i: in heap w[i] (-1: none) at it[i]
        vt<PQ> s; vt<PQ::point_iterator> it; vi w;
        void init(int h, int m) {
            s.assign(h, {}), it.assign(m, {}), w.assign(m, -1);
        }
        void er(int i) {
            if (w[i] >= 0) s[w[i]].erase(it[i]), w[i] = -1;
        }
        void put(int i, A v, int h = 0) {
            v[3] = i;
            if (w[i] == h) return s[h].modify(it[i], v);
            er(i), it[i] = s[h].push(v), w[i] = h;
        }
        A top(int h = 0) {
            return s[h].empty() ? A{INF} : s[h].top();
        }
        void clear(int h = 0) {
            for (A a : s[h]) w[a[3]] = -1;
            s[h].clear();
        }
    };
    int n, qh = 0, cc = 0; ll T = 0; pl e1 = {INF, 0};
    vi mate, sf, bs, lab, hv, gp, bf, fr, q, sz;
    vl pot, tc, lz, sl; Heaps h2, h2s, h4;
    priority_queue<A, vt<A>, greater<A>> h3;
    vt<vt<pl>> g; vt<pi> lk; vt<array<pi, 2>> nd;
    void init(int _n) { n = _n, g.assign(n + 1, {}); }
    void ae(int u, int v, ll w) {
        g[u].pb({v, 2 * w}), g[v].pb({u, 2 * w});
    }
    void rematch(int v, int w) {
        int t = mate[v]; mate[v] = w; if (mate[t] != v) return;
        auto [x, y] = lk[v];
        if (y == sf[y]) mate[t] = x, rematch(x, t);
        else rematch(x, y), rematch(y, x);
    }
    void fix(int b) { // mate and base of b after augmenting
        if (b <= n) return;
        int v = bs[b], d = nd[nd[v][0].f][1].s != mate[nd[v][0].s];
        for (int m; nd[m = nd[v][d].f][!d].s == mate[nd[v][d].s];
                v = nd[m][d].f) fix(v), fix(m);
        fix(bs[b] = v), mate[b] = mate[v];
    }
    ll settle(int b, bool in) { // move lazy potential into b
        ll k = in * (T - tc[b]), d = lz[b] + k;
        return lz[b] = 0, pot[b] -= 2 * k * (b > n), d;
    }
    void upd(int x, int y, int by, ll t, bool inner) {
        if (t >= sl[y]) return;
        sl[y] = t, bf[y] = x;
        if (y != by) {
            int gy = gp[y];
            if (gy != y && t >= sl[gy]) return;
            sl[gy] = t, h2s.put(gy, {t, x, y}, by);
        }
        if (inner) return;
        A m = y == by ? A{t, x, y} : h2s.top(by); m[0] += lz[by];
        if (h2.w[by] < 0 || m[0] < (*h2.it[by])[0]) h2.put(by, m);
    } // <hash>
    void swp(int a, int b) { // b is a maximal blossom
        auto w = [&](auto&... v) { (swap(v[a], v[b]), ...); };
        F0R (d, 2) nd[nd[a][d].f][!d].f = b;
        w(lk, nd, mate, sz, pot, lz, tc, bs, hv);
        for (auto v : {&bs, &hv}) if ((*v)[a] == a) (*v)[a] = b;
    }
    void setsf(int b, int s, int g2) {
        sf[b] = s, gp[b] = g2;
        for (int c = bs[b]; sf[c] != s; c = nd[c][0].f)
            setsf(c, s, g2);
    }
    void contract(int x, int y) {
        int bx = sf[x], by = sf[y], h = -1 - cc++, l;
        auto mk = [&](int b) -> int& { return lk[sf[mate[b]]].f; };
        mk(bx) = mk(by) = h;
        do {
            if (mate[by]) swap(bx, by);
            bx = l = sf[lk[bx].f];
        } while (mk(bx) - h && (mk(bx) = h));
        int id = fr.back(), ts = 0; fr.pop_back();
        F0R (d, 2) {
            for (int bv = sf[x]; bv != l;) {
                int mv = mate[bv], bm = sf[mv], v = mate[mv];
                auto [f, t] = lk[v];
                ts += sz[bv] + sz[bm], lk[mv] = {x, y}, h4.er(bm);
                if (bv > n) pot[bv] += 2 * (T - tc[bv]);
                outer(bm, settle(bm, 1));
                nd[bv][d] = {bm, mv}, nd[bm][!d] = {bv, v};
                nd[bm][d] = {bv = sf[f], f}, nd[bv][!d] = {bm, t};
            }
            nd[sf[x]][!d] = {sf[y], y};
            swap(x, y);
        }
        if (l > n) pot[l] += 2 * (T - tc[l]);
        sz[id] = ts + sz[l], bs[id] = l, lk[id] = lk[l];
        mate[id] = mate[l], lab[id] = 1, sf[id] = id;
        tc[id] = T, pot[id] = lz[id] = hv[id] = 0;
        int big = id, bsz = 1, c = l; // heavy child keeps its id
        do if (sz[c] > bsz) bsz = sz[c], big = c;
        while ((c = nd[c][0].f) != l);
        do if (c != big) setsf(c, big, c);
        while ((c = nd[c][0].f) != l);
        if (bsz > 1) sf[id] = hv[id] = big, swp(big, id);
    } // <hash>
    void sched(int b) { h4.put(b, {T + (pot[b] >> 1)}); }
    void link(int v, pi p) {
        lk[v] = p;
        if (v <= n) return;
        int b = bs[v]; link(b, p);
        p = {nd[nd[b][1].f][0].s, nd[b][1].s};
        for (int c = b, w; (w = nd[c][0].f) != b; c = nd[w][0].f) {
            link(w, p);
            link(nd[w][0].f, {nd[w][1].s, nd[c][0].s});
        }
    }
    void outer(int v, ll d) {
        lab[v] = 1;
        if (v > n)
            for (int b = bs[v]; lab[b] != 1; b = nd[b][0].f)
                outer(b, d);
        else {
            pot[v] += T + d, q.pb(v);
            if (pot[v] < e1.f) e1 = {pot[v], v};
        }
    }
    bool grow(int x, int y) {
        int by = sf[y], vis = lab[by], z = mate[by], bz = sf[z];
        if (!vis) link(by, {0, 0});
        lab[by] = -1, tc[by] = T, h2.er(by);
        if (y != by) sched(by);
        if (!z) return rematch(x, y), rematch(y, x), 1;
        if (!vis) link(bz, {x, y}); else lk[bz] = lk[z] = {x, y};
        return outer(bz, settle(bz, 0)), tc[bz] = T, h2.er(bz), 0;
    } // <hash>
    int ms(int b) { // vertex under b with least slack
        if (b <= n) return b;
        int v = 0, c = bs[b], w;
        do if (sl[w = ms(c)] < sl[v]) v = w;
        while ((c = nd[c][0].f) != bs[b]);
        return v;
    }
    void split(int b, int s, int g2) {
        sf[b] = s, gp[b] = g2;
        for (int c = bs[b]; sf[c] != s; c = nd[c][0].f)
            if (c == hv[b]) split(c, s, g2);
            else {
                setsf(c, s, c);
                int to = ms(c);
                if ((sl[c] = sl[to]) < INF)
                    h2s.put(c, {sl[c], bf[to], to}, s);
            }
    }
    void expand(int id) {
        int mv = mate[bs[id]], h = hv[id], c = bs[id];
        ll d = settle(id, 1);
        do {
            tc[c] = T, lz[c] = d;
            if (c != h) split(c, c, c), h2s.er(c);
        } while ((c = nd[c][0].f) != bs[id]);
        if (h > 0) swp(h, id), id = h;
        fr.pb(id), bs[id] = id, h2s.clear(id);
        pi ol = lk[mv]; int ob = sf[mate[mv]], rt = sf[ol.s];
        int e = mate[rt] == nd[rt][0].s;
        for (int b = nd[ob][!e].f; b != rt; b = nd[b][!e].f) {
            lab[b] = -2;
            A m = b <= n ? A{sl[b], bf[b], b} : h2s.top(b);
            if (m[0] < INF) m[0] += lz[b], h2.put(b, m);
        }
        for (int b = ob;; b = nd[b][e].f) {
            lab[b] = -1;
            int nb = nd[b][e].f;
            pi p = b == rt ? ol : pi{nd[b][e].s, nd[nb][!e].s};
            lk[mate[b]] = lk[sf[mate[b]]] = p;
            if (b > n) pot[b] ? sched(b) : expand(b);
            if (b == rt) break;
            outer(nb, settle(b = nb, 1));
        }
    } // <hash>
    bool path() {
        while (qh < size(q)) {
            int x = q[qh++], bx = sf[x];
            if (pot[x] == T) return rematch(x, 0), 1;
            for (auto [y, c] : g[x]) {
                int by = sf[y];
                ll l = lab[by], t = pot[x] + pot[y] - c;
                if (bx == by) continue;
                if (l == 1) {
                    if (t >> 1 == T) contract(x, y), bx = sf[x];
                    else if (t >> 1 < e1.f) h3.push({t >> 1, x, y});
                } else if (l + 1 && t + lz[by] == T) {
                    if (grow(x, y)) return 1;
                } else if (l + 1 || mate[x] != y)
                    upd(x, y, by, t, l == -1);
            }
        }
        return 0;
    }
    A pk() { // best live blossom-merging edge, {INF} if none
        while (size(h3) && sf[h3.top()[1]] == sf[h3.top()[2]])
            h3.pop();
        return size(h3) ? h3.top() : A{INF};
    }
    bool dual() { // 1: root matched or can stay unmatched
        T = min({e1.f, h2.top()[0], pk()[0], h4.top()[0]});
        if (T == e1.f) return rematch(e1.s, 0), 1;
        for (A e; (e = h2.top())[0] == T;)
            if (grow(e[1], e[2])) return 1;
        for (A e; (e = pk())[0] == T; h3.pop())
            contract(e[1], e[2]);
        for (A e; (e = h4.top())[0] == T;)
            h4.er(e[3]), expand(e[3]);
        return 0;
    } // <hash>
    ll solve() {
        int S = n + (n - 1) / 2 + 1;
        mate = lab = hv = bf = vi(S), sz = vi(S, 1);
        sf = vi(S), iota(all(sf), 0), bs = gp = sf;
        fr = vi(sf.rbegin(), sf.rend() - n - 1);
        lk.assign(S, {}), nd.assign(S, {}), sl.assign(S, INF);
        pot = tc = lz = vl(S);
        h2.init(1, S), h2s.init(S, S), h4.init(1, S);
        FOR (u, 1, n + 1) for (auto [v, c] : g[u])
            pot[u] = max(pot[u], c / 2);
        FOR (u, 1, n + 1) if (!mate[u] && pot[u]) {
            link(sf[u], {0, 0}), outer(sf[u], 0);
            while (!path() && !dual());
            FOR (v, 0, n + 1) {
                int b = sf[v];
                if (lab[v] == 1) pot[v] -= T;
                else pot[v] += lz[b] + (lab[b] == -1) * (T - tc[b]);
                lab[v] = lk[v].f = lz[v] = 0, sl[v] = INF;
            }
            for (int b = n + 1, r = S - n - 1 - size(fr);
                    r && b < S; b++)
                if (bs[b] != b) {
                    if (sf[b] == b) fix(b), pot[b] += 2 * (T - tc[b]) *
                        ((lab[b] == 1) - (lab[b] == -1));
                    h2s.clear(b), r--;
                    lab[b] = lk[b].f = lz[b] = 0, sl[b] = INF;
                }
            q.clear(), qh = cc = 0, T = 0, e1 = {INF, 0};
            h2.clear(), h4.clear(), h3 = {};
        }
        vl m(n + 1);
        FOR (u, 1, n + 1) for (auto [v, w] : g[u])
            if (v == mate[u]) m[u] = max(m[u], w);
        return accumulate(all(m), 0LL) / 4;
    } // <hash>
};
