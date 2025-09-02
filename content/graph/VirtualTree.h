/**
 * Author: Simon Lindholm
 * Date: 2016-01-14
 * License: CC0
 * Description: Computes virtual tree.
 *  \texttt{pos} is inorder dfs time, and returns pairs of \texttt{(par, child)}.
 * Time: $O(|S| \log |S|)$
 * Status: Tested at CodeForces
 */
#pragma once

#include "LCA.h"

// pos is dfs time
// pairs of {ancestor, child}
vt<pl> virtual_tree(vt<ll>& nodes) { 
    auto cmp = [&] (ll u, ll v) { return pos[u] < pos[v]; };
    sort(all(nodes), cmp);
    int sz = size(nodes);
    FOR (i, sz - 1) nodes.pb(lca(nodes[i], nodes[i + 1]));
    sort(all(nodes), cmp);
    nodes.erase(unique(all(nodes)), nodes.end());
    vt<pl> res;
    FOR (i, (int) size(nodes) - 1) 
    res.pb({lca(nodes[i], nodes[i + 1]), nodes[i + 1]});
    return res;
}