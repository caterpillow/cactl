/**
 * Author: Chen Xing
 * Date: 2009-10-13
 * License: CC0
 * Source: N/A
 * Description: Flow algorithm with guaranteed complexity $O(VE^2)$. To get edge flow values, compare
 * capacities before and after, and take the positive values only.
 * Status: stress-tested
 */
#pragma once

template<class T> T edmondsKarp(vector<unordered_map<int, T>>&
		adj, int s, int t) {
	assert(s != t);
	T flow = 0;
	vt<int> par(size(adj)), q = par;
    while (1) {
		fill(all(par), -1);
		int ptr = 1;
		q[0] = s, par[s] = 0;
        F0R (i, ptr) {
			int u = q[i];
            for (auto &[v, c] : adj[u]) {
				if (par[v] == -1 && c) {
					par[v] = u, q[ptr++] = v;
					if (v == t) goto out;
				}
			}
		}
        return flow;
    out:
		T inc = numeric_limits<T>::max();
		for (int y = t; y != s; y = par[y])
			inc = min(inc, adj[par[y]][y]);

		flow += inc;
		for (int y = t; y != s; y = par[y]) {
			int p = par[y];
			if ((adj[p][y] -= inc) <= 0) adj[p].erase(y);
			adj[y][p] += inc;
		}
	}
}