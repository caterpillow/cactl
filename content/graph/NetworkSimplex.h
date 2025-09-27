/**
 * Author: idk
 * Date: 2025-09-03 
 * Source: atcoder submission
 * Description: Solves minimum cost circulation problem.
 *  To convert to a "normal" mcmf, add an edge from \text{t -> s} of big capacity and negative inf cost (remember to add this back on to answer!).
 *  If you don't necessarily need to maximise flow, add free edge from \text{s -> t}.
 *  Edge $i$ (one indexed) is \text{ns.edges[2 * i]]}.
 *  Works with negative cost cycles.
 * Status: seems to print the right answer at least
 * Time: $O(VE)$ on average maybe
 */
#pragma once

struct NetworkSimplex {
    using Flow = int;
    using Cost = ll;
    struct Edge {
        int nxt, to;
        Flow cap;
        Cost cost;
    };
    vector<Edge> edges;
    vector<int> head, fa, fe, mark, cyc;
    vector<Cost> dual;
    int ti;

    NetworkSimplex(int n)
            : head(n, 0), fa(n), fe(n), mark(n), cyc(n + 1), dual(n), ti(0) {
        edges.push_back({0, 0, 0, 0});
        edges.push_back({0, 0, 0, 0});
    }

    int ae(int u, int v, Flow cap, Cost cost) {
        assert(size(edges) % 2 == 0);
        int e = size(edges);
        edges.push_back({head[u], v, cap, cost});
        head[u] = e;
        edges.push_back({head[v], u, 0, -cost});
        head[v] = e + 1;
        return e;
    }

    void init_tree(int x) {
        mark[x] = 1;
        for (int i = head[x]; i; i = edges[i].nxt) {
            int v = edges[i].to;
            if (!mark[v] and edges[i].cap) {
                fa[v] = x, fe[v] = i;
                init_tree(v);
            }
        }
    }

    int phi(int x) {
        if (mark[x] == ti) return dual[x];
        return mark[x] = ti, dual[x] = phi(fa[x]) - edges[fe[x]].cost;
    }

    void push_flow(int e, Cost &cost) {
        int pen = edges[e ^ 1].to, lca = edges[e].to;
        ti++;
        while (pen) mark[pen] = ti, pen = fa[pen];
        while (mark[lca] != ti) mark[lca] = ti, lca = fa[lca];

        int e2 = 0, f = edges[e].cap, path = 2, clen = 0;
        for (int i = edges[e ^ 1].to; i != lca; i = fa[i]) {
            cyc[++clen] = fe[i];
            if (edges[fe[i]].cap < f)
                f = edges[fe[e2 = i] ^ (path = 0)].cap;
        }
        for (int i = edges[e].to; i != lca; i = fa[i]) {
            cyc[++clen] = fe[i] ^ 1;
            if (edges[fe[i] ^ 1].cap <= f)
                f = edges[fe[e2 = i] ^ (path = 1)].cap;
        }
        cyc[++clen] = e;

        for (int i = 1; i <= clen; ++i) {
            edges[cyc[i]].cap -= f, edges[cyc[i] ^ 1].cap += f;
            cost += edges[cyc[i]].cost * f;
        }
        if (path == 2)  return;

        int laste = e ^ path, last = edges[laste].to, cur = edges[laste ^ 1].to;
        while (last != e2) {
            mark[cur]--;
            laste ^= 1;
            swap(laste, fe[cur]);
            swap(last, fa[cur]); swap(last, cur);
        }
    }

    Cost compute() {
        Cost cost = 0;
        init_tree(0);
        mark[0] = ti = 2, fa[0] = cost = 0;
        int ncnt = size(edges) - 1;
        for (int i = 2, pre = ncnt; i != pre; i = i == ncnt ? 2 : i + 1)
            if (edges[i].cap and edges[i].cost < phi(edges[i ^ 1].to) - phi(edges[i].to))
                push_flow(pre = i, cost);
        ti++;
        for (int u = 0; u < size(dual); ++u)
            phi(u);
        return cost;
    }
};