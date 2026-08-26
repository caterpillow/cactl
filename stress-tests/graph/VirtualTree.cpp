// Tests VirtualTree.h: builds virtual trees over random subsets of random trees
// and checks the edge list vs brute (closure under pairwise LCA; each node's
// parent = deepest proper ancestor in the closure). Sets up the required
// globals pos + lca(u, v) from LCA.h. written by Claude (audit)
#include "../utilities/template.h"
#include "../utilities/genTree.h"
#include "../../content/graph/LCA.h"

LCA* L;
vi pos;
ll lca(ll u, ll v) { return (*L)((int) u, (int) v); }

#include "../../content/graph/VirtualTree.h"

vi par_, dep_;
void root_dfs(vt<vi>& adj, int u, int p, int d) {
	par_[u] = p, dep_[u] = d;
	for (int v : adj[u]) if (v != p) root_dfs(adj, v, u, d + 1);
}
bool is_anc(int a, int b) { // a ancestor of b (inclusive)
	while (b != -1) { if (b == a) return true; b = par_[b]; }
	return false;
}
int main() {
	srand(999);
	F0R (it, 5000) {
		int n = rand() % 50 + 1;
		vt<vi> adj(n);
		if (n > 1) for (auto [a, b] : genRandomTree(n)) adj[a].pb(b), adj[b].pb(a);
		par_ = dep_ = vi(n);
		root_dfs(adj, 0, -1, 0);
		LCA lc(adj);
		L = &lc; pos = lc.time;
		int k = rand() % n + 1;
		set<ll> S;
		F0R (i, k) S.insert(rand() % n);
		vt<ll> nodes(all(S));
		auto res = virtual_tree(nodes);
		// brute closure under pairwise lca
		set<int> clo;
		for (ll x : S) clo.insert((int) x);
		for (ll a : S) for (ll b : S) clo.insert((int) lca(a, b));
		// expected edges: parent = deepest proper ancestor in closure
		set<pl> exp;
		for (int x : clo) {
			int best = -1;
			for (int y : clo) if (y != x && is_anc(y, x))
				if (best == -1 || dep_[y] > dep_[best]) best = y;
			if (best != -1) exp.insert({best, x});
		}
		set<pl> got(all(res));
		assert(size(res) == size(got));       // no duplicate edges
		assert(got == exp);                   // exact minimal edge set
		assert(size(got) == size(clo) - 1);   // forms a tree over the closure
	}
	cout << "Tests passed!" << endl;
}
