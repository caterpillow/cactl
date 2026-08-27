/**
 * Author: chilli, pajenegod
 * Date: 2020-02-20
 * License: CC0
 * Source: Folklore
 * Description: Data structure for computing lowest common ancestors in a tree
 * (with 0 as root). C should be an adjacency list of the tree, either directed
 * or undirected.
 * Time: $O(N \log N + Q)$
 * Status: stress-tested
 */
#pragma once

#include "../data-structures/SparseTable.h"

struct LCA {
	int t = 0;
	vi time, path, ret;
	RMQ<int> rmq;

	// n == 1: ret is empty and RMQ asserts; special-case it
	LCA(vt<vi>& adj) : time(size(adj)) { dfs(0, -1, adj); rmq.init(ret); }
	void dfs(int u, int p, vt<vi> &adj) {
		time[u] = t++;
		for (int v : adj[u]) if (v != p) {
			path.pb(u), ret.pb(time[u]);
			dfs(v, u, adj);
		}
	}

	int operator()(int u, int v) {
		if (u == v) return u;
		tie(u, v) = minmax(time[u], time[v]);
		return path[rmq.query(u, v)];
	}
};
