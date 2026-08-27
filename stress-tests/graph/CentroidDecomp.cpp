// Tests CentroidDecomp.h (paste-into-main snippet, needs adj): simulates
// processing vertices in `ord` with each dfs stopping at processed vertices
// and checks (1) every vertex is the unique max-label vertex of its remaining
// component (so no two same-label vertices share a component), (2) max label
// <= log2(n), i.e. decomposition depth <= log2(n)+1 and total dfs work
// <= n(log2(n)+1). NOTE: it does NOT pick true centroids (path 0-1-2-3 picks
// vertex 0 first); only the depth bound holds. written by Claude (audit)
#include "../utilities/template.h"
#include "../utilities/genTree.h"
int main() {
	srand(7);
	F0R (it, 20000) {
		int n = rand() % 40 + 1, kind = rand() % 3;
		vt<vi> adj(n);
		auto add = [&](int a, int b) { adj[a].pb(b), adj[b].pb(a); };
		if (kind == 0) { if (n > 1) for (auto [a, b] : genRandomTree(n)) add(a, b); }
		else if (kind == 1) FOR (i, 1, n) add(i - 1, i);
		else FOR (i, 1, n) add(0, i);
		#include "../../content/graph/CentroidDecomp.h"
		vi lab(n);
		for (auto [nl, u] : ord) lab[u] = -nl;
		assert(*max_element(all(lab)) <= __lg(n));
		vt<bool> done(n);
		ll total = 0;
		for (auto [nl, u] : ord) {
			vi st = {u}; vt<bool> seen(n); seen[u] = 1;
			while (size(st)) {
				int x = st.back(); st.pop_back(); total++;
				assert(x == u || lab[x] < lab[u]);
				for (int y : adj[x]) if (!done[y] && !seen[y]) seen[y] = 1, st.pb(y);
			}
			done[u] = 1;
		}
		assert(total <= (ll) n * (__lg(n) + 1));
	}
	cout << "Tests passed!" << endl;
}
