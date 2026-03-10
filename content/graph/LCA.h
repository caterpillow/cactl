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

#include "../data-structures/RMQ.h"

struct LCA {
	int t = 0;
	vi time, path, ret;
	RMQ<int> rmq;

	LCA(vt<vi>& adj) : time(size(adj)), rmq((dfs(0, -1, adj), ret)) {}
	void dfs(int u, int p, vt<vi> &adj) {
		time[u] = t++;
		for (int v : adj[u]) if (v != p) {
			path.push_back(u), ret.push_back(time[u]);
			dfs(v, u, adj);
		}
	}

	int operator()(int u, int v) {
		if (u == v) return u;
		tie(u, v) = minmax(time[u], time[v]);
		return path[rmq.query(u, v)];
	}
};
