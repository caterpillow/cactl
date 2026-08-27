/**
 * Author: Benq
 * Description: Used only a few times. Assuming that all nodes are
 * reachable from $root,$ $a$ dominates $b$ iff every path from
 * $root$ to $b$ passes through $a.$ \texttt{ans[a]} lists the children of
 * $a$ in the dominator tree (immediate dominees).
 * Usage: Dominator d; d.init(n); d.ae(a, b); d.build(root);
 * Time: O(M\log N)
 * Source: https://tanujkhattar.wordpress.com/2016/01/11/dominator-tree-of-a-directed-graph/
 * Status: stress-tested
 */
#pragma once

struct Dominator {
    int co;
    vt<vi> adj, ans; // input edges, edges of dominator tree
    vt<vi> radj, child, sdomChild;
    vi label, rlabel, sdom, dom, par, bes;
    void init(int n) {
        co = 0;
        adj = ans = radj = child = sdomChild = vt<vi>(n + 1);
        label = rlabel = sdom = dom = par = bes = vi(n + 1);
    }
    void ae(int a, int b) { adj[a].pb(b); }
    int get(int x) { // DSU with path compression
        // get vertex with smallest sdom on path to root
        if (par[x] != x) {
            int t = get(par[x]); par[x] = par[par[x]];
            if (sdom[t] < sdom[bes[x]]) bes[x] = t;
        }
        return bes[x];
    }
    void dfs(int x) { // create DFS tree
        label[x] = ++co; rlabel[co] = x;
        sdom[co] = par[co] = bes[co] = co;
        for (int y : adj[x]) {
            if (!label[y])
                dfs(y), child[label[x]].pb(label[y]);
            radj[label[y]].pb(label[x]);
        }
    }
    void build(int root) {
        dfs(root);
        ROF (i, 1, co + 1) {
            for (int j : radj[i])
                sdom[i] = min(sdom[i], sdom[get(j)]);
            if (i > 1) sdomChild[sdom[i]].pb(i);
            for (int j : sdomChild[i]) {
                int k = get(j);
                dom[j] = sdom[j] == sdom[k] ? sdom[j] : k;
            }
            for (int j : child[i]) par[j] = i;
        }
        FOR (i, 2, co + 1) {
            if (dom[i] != sdom[i]) dom[i] = dom[dom[i]];
            ans[rlabel[dom[i]]].pb(rlabel[i]);
        }
    }
};
