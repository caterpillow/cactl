// Tests LCA.h (RMQ-based, new init() constructor) all-pairs vs brute parent-climbing
// on random trees (2<=n<=60), plus star and path shapes. written by Claude (audit)
#include "../utilities/template.h"
#include "../utilities/genTree.h"
#include "../../content/graph/LCA.h"

vi par_, dep_;
void root_dfs(vt<vi>& adj, int u, int p, int d) {
	par_[u] = p, dep_[u] = d;
	for (int v : adj[u]) if (v != p) root_dfs(adj, v, u, d + 1);
}
int brute_lca(int a, int b) {
	while (dep_[a] > dep_[b]) a = par_[a];
	while (dep_[b] > dep_[a]) b = par_[b];
	while (a != b) a = par_[a], b = par_[b];
	return a;
}
void check(vt<vi>& adj) {
	int n = size(adj);
	par_ = dep_ = vi(n);
	root_dfs(adj, 0, -1, 0);
	LCA lca(adj);
	F0R (a, n) F0R (b, n) assert(lca(a, b) == brute_lca(a, b));
}
int main() {
	srand(1234);
	{ // star and path, n = 60
		vt<vi> star(60), path(60);
		FOR (i, 1, 60) star[0].pb(i), star[i].pb(0);
		FOR (i, 1, 60) path[i - 1].pb(i), path[i].pb(i - 1);
		check(star); check(path);
	}
	F0R (it, 3000) {
		int n = rand() % 60 + 2; // n == 1 is the documented special case
		vt<vi> adj(n);
		if (n > 1) for (auto [a, b] : genRandomTree(n)) adj[a].pb(b), adj[b].pb(a);
		check(adj);
	}
	cout << "Tests passed!" << endl;
}
