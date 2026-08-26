// Tests HLD.h process()-based API: path add + path max (via a plain array range
// structure over pos), in_edges = false and true, vs brute parent-climbing.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../utilities/genTree.h"
#undef sz // legacy shim collides with HLD's 'sz' member
#include "../../content/graph/HLD.h"

vi par_, dep_;
void root_dfs(vt<vi>& adj, int u, int p, int d) {
	par_[u] = p, dep_[u] = d;
	for (int v : adj[u]) if (v != p) root_dfs(adj, v, u, d + 1);
}
// path from a to b as vertex list
vi path_verts(int a, int b) {
	vi A, B;
	while (dep_[a] > dep_[b]) A.pb(a), a = par_[a];
	while (dep_[b] > dep_[a]) B.pb(b), b = par_[b];
	while (a != b) A.pb(a), B.pb(b), a = par_[a], b = par_[b];
	A.pb(a);
	reverse(all(B));
	for (int x : B) A.pb(x);
	return A;
}
template<bool in_edges> void run(int trees, int ops) {
	F0R (it, trees) {
		int n = rand() % 60 + 1;
		vt<vi> adj(n);
		if (n > 1) for (auto [a, b] : genRandomTree(n)) adj[a].pb(b), adj[b].pb(a);
		par_ = dep_ = vi(n);
		root_dfs(adj, 0, -1, 0);
		vt<vi> cp = adj;
		HLD<in_edges> hld(cp);
		vl arr(n, 0);          // range structure indexed by pos
		vl bval(n, 0);         // brute: value of vertex v / edge (v, par_[v])
		F0R (op, ops) {
			int a = rand() % n, b = rand() % n, val = rand() % 100;
			// brute node set on path (edges: drop the lca)
			vi p = path_verts(a, b);
			if (in_edges) { // drop the lca: remaining vertices stand for their parent edges
				int lc = p[0]; for (int x : p) if (dep_[x] < dep_[lc]) lc = x;
				vi q; for (int x : p) if (x != lc) q.pb(x);
				p = q;
			}
			if (rand() % 2) { // path add
				hld.process(a, b, [&](int l, int r) { FOR (i, l, r) arr[i] += val; });
				for (int x : p) bval[x] += val;
			} else { // path max
				ll mx = -INF;
				hld.process(a, b, [&](int l, int r) { FOR (i, l, r) mx = max(mx, arr[i]); });
				ll bmx = -INF;
				for (int x : p) bmx = max(bmx, bval[x]);
				assert(mx == bmx);
			}
		}
	}
}
int main() {
	srand(777);
	run<false>(500, 60);
	run<true>(500, 60);
	cout << "Tests passed!" << endl;
}
