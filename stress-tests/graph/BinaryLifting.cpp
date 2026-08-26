// Tests BinaryLifting.h: jump() vs step-by-step climbing and lca() vs brute,
// on random trees. written by Claude (audit)
#include "../utilities/template.h"
#include "../utilities/genTree.h"
#include "../../content/graph/BinaryLifting.h"

vi par_, dep_;
void root_dfs(vt<vi>& adj, int u, int p, int d) {
	par_[u] = max(p, 0), dep_[u] = d;
	for (int v : adj[u]) if (v != p) root_dfs(adj, v, u, d + 1);
}
int brute_lca(int a, int b) {
	while (dep_[a] > dep_[b]) a = par_[a];
	while (dep_[b] > dep_[a]) b = par_[b];
	while (a != b) a = par_[a], b = par_[b];
	return a;
}
int main() {
	srand(4321);
	F0R (it, 1000) {
		int n = rand() % 60 + 1;
		vt<vi> adj(n);
		if (n > 1) for (auto [a, b] : genRandomTree(n)) adj[a].pb(b), adj[b].pb(a);
		par_ = dep_ = vi(n);
		root_dfs(adj, 0, -1, 0);
		auto jmp = build_table(par_);
		F0R (q, 100) {
			int u = rand() % n, d = rand() % n;
			int w = u;
			F0R (i, d) w = par_[w];
			assert(jump(jmp, u, d) == w);
			int a = rand() % n, b = rand() % n;
			assert(lca(jmp, dep_, a, b) == brute_lca(a, b));
		}
	}
	cout << "Tests passed!" << endl;
}
