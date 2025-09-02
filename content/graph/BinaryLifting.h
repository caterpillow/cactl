/**
 * Author: Johan Sannemo
 * Date: 2015-02-06
 * License: CC0
 * Source: Folklore
 * Description: Calculate power of two jumps in a tree,
 * to support fast upward jumps and LCAs.
 * Assumes the root node points to itself.
 * Time: construction $O(N \log N)$, queries $O(\log N)$
 * Status: Tested at Petrozavodsk, also stress-tested via LCA.cpp
 */
#pragma once

vt<vt<int>> build_table(vt<int>& par){
	int d = 1;
	while ((1 << d) < sz(par)) d++;
	vt<vt<int>> jmp(d, par);
	FOR (i, 1, d) FOR (j, 0, size(par))
		jmp[i][j] = jmp[i - 1][jmp[i - 1][j]];
	return jmp;
}

int jump(vt<vt<int>>& jmp, int u, int d){
	F0R (i, size(jmp))
		if (1 & d >> i) u = jmp[i][u];
	return u;
}

int lca(vt<vt<int>>& jmp, vt<int>& depth, int a, int b) {
	if (depth[a] < depth[b]) swap(a, b);
	a = jump(jmp, a, depth[a] - depth[b]);
	if (a == b) return a;
	for (int i = size(jmp); i--;) {
		int c = jmp[i][a], d = jmp[i][b];
		if (c != d) a = c, b = d;
	}
	return jmp[0][a];
}
