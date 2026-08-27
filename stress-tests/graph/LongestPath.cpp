// Tests LongestPath.h (standalone program: reads a tree, prints for each
// node the max distance to any other node) against BFS on random trees.
// The header's main() is renamed to a void lp_main() via #define and run
// with cin/cout redirected to stringstreams.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../utilities/random.h"

// 'int main() {' -> 'int lp_dummy(); void lp_main() {' (void: no missing-return UB)
#define main lp_dummy(); void lp_main
#include "../../content/graph/LongestPath.h"
#undef main

int main() {
	ios::sync_with_stdio(0); // makes the header's own call a no-op, keeping our rdbufs
	rep(it,0,3000) {
		int n = rand() % 12 + 1;
		vi perm(n); iota(all(perm), 0); shuffle_vec(perm);
		vector<pii> ed; vector<vi> adj(n);
		int mode = rand() % 3;
		rep(i,1,n) {
			int p = mode == 0 ? rand() % i : mode == 1 ? i - 1 : max(0, i - 1 - rand() % 3);
			int a = perm[i], b = perm[p];
			if (randBool()) swap(a, b);
			ed.push_back({a, b}); adj[a].push_back(b); adj[b].push_back(a);
		}
		shuffle_vec(ed);
		stringstream in, out, exp;
		in << n << '\n';
		for (auto [a, b] : ed) in << a + 1 << ' ' << b + 1 << '\n';
		rep(s,0,n) {
			vi d(n, -1); d[s] = 0; queue<int> q; q.push(s); int mx = 0;
			while (!q.empty()) {
				int u = q.front(); q.pop(); mx = max(mx, d[u]);
				for (int v : adj[u]) if (d[v] < 0) d[v] = d[u] + 1, q.push(v);
			}
			exp << mx << " \n"[s == n - 1];
		}
		auto ci = cin.rdbuf(in.rdbuf()); auto co = cout.rdbuf(out.rdbuf());
		lp_main();
		cin.rdbuf(ci); cout.rdbuf(co);
		assert(out.str() == exp.str());
	}
	cout << "Tests passed!" << endl;
}
