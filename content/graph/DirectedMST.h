/**
 * Author: chilli, Takanori MAEHARA, Benq, Simon Lindholm
 * Date: 2019-05-10
 * License: CC0
 * Source: https://github.com/spaghetti-source/algorithm/blob/master/graph/arborescence.cc
 * and https://github.com/bqi343/USACO/blob/42d177dfb9d6ce350389583cfa71484eb8ae614c/Implementations/content/graphs%20(12)/Advanced/DirectedMST.h for the reconstruction
 * Description: Finds a minimum spanning
 * tree/arborescence of a directed graph, given a root node. If no MST exists, returns -1.
 * Time: O(E \log V)
 * Status: Stress-tested, also tested on NWERC 2018 fastestspeedrun
 */
#pragma once

#include "../data-structures/DSURollback.h"

struct Edge { int a, b; ll w; };
struct Node { /// lazy skew heap node
    Edge key;
    Node *l, *r;
    ll delta;
    void prop() {
        key.w += delta;
        if (l) l->delta += delta;
        if (r) r->delta += delta;
        delta = 0;
    }
    Edge top() { prop(); return key; }
};
Node *merge(Node *a, Node *b) {
    if (!a || !b) return a ?: b;
    a->prop(), b->prop();
    if (a->key.w > b->key.w) swap(a, b);
    swap(a->l, (a->r = merge(b, a->r)));
    return a;
}
void pop(Node*& a) { a->prop(); a = merge(a->l, a->r); }

pair<ll, vi> dmst(int n, int r, vt<Edge>& g) {
    DSU uf; uf.init(n);
    vt<Node*> heap(n);
    for (Edge e : g) heap[e.b] = merge(heap[e.b], new Node{e});
    ll res = 0;
    vi seen(n, -1), path(n), par(n);
    seen[r] = r;
    vt<Edge> Q(n), in(n, {-1, -1}), comp;
    deque<pair<int, vt<Edge>>> cycs;
    F0R (s, n) {
        int u = s, qi = 0, w;
        while (seen[u] < 0) {
            if (!heap[u]) return {-1, {}};
            Edge e = heap[u]->top();
            heap[u]->delta -= e.w, pop(heap[u]);
            Q[qi] = e, path[qi++] = u, seen[u] = s;
            res += e.w, u = uf.find(e.a);
            if (seen[u] == s) { /// found cycle, contract
                Node* cyc = 0;
                int end = qi;
                uf.push(); // checkpoint, undone in the restore loop
                do cyc = merge(cyc, heap[w = path[--qi]]);
                while (uf.unite(u, w));
                u = uf.find(u), heap[u] = cyc, seen[u] = -1;
                cycs.push_front({u, {&Q[qi], &Q[end]}});
            }
        }
        F0R (i, qi) in[uf.find(Q[i].b)] = Q[i];
    }

    for (auto& [u, comp] : cycs) { // restore sol (optional)
        uf.pop();
        Edge inEdge = in[u];
        for (auto& e : comp) in[uf.find(e.b)] = e;
        in[uf.find(inEdge.b)] = inEdge;
    }
    F0R (i, n) par[i] = in[i].a;
    return {res, par};
}
