// Tests DominatorTree.h: immediate dominators vs brute force (a dominates b
// iff b is unreachable from root once a is deleted). Written by Claude (audit).
#include "../utilities/template.h"
#include "../../content/graph/DominatorTree.h"
mt19937 rng(11);
int main() {
	F0R (it, 5000) {
		int n = rng() % 9 + 1, m = rng() % (3 * n + 1);
		vt<pi> ed;
		FOR (v, 1, n) ed.pb({(int) (rng() % v), v}); // keep all reachable from 0
		F0R (i, m) ed.pb({(int) (rng() % n), (int) (rng() % n)});
		auto reach = [&](int skip) { // reachable from 0 avoiding skip
			vi vis(n); vi st; if (skip != 0) vis[0] = 1, st.pb(0);
			while (size(st)) { int u = st.back(); st.pop_back();
				for (auto [a, b] : ed) if (a == u && b != skip && !vis[b]) vis[b] = 1, st.pb(b); }
			return vis;
		};
		vt<vi> doms(n); // doms[b] = vertices dominating b (excluding b)
		F0R (a, n) { vi r = reach(a); F0R (b, n) if (b != a && !r[b]) doms[b].pb(a); }
		Dominator d; d.init(n);
		for (auto [a, b] : ed) d.ae(a, b);
		d.build(0);
		vi idom(n, -1);
		F0R (a, n) for (int b : d.ans[a]) assert(idom[b] == -1), idom[b] = a;
		FOR (b, 1, n) { // idom = the dominator dominated by all other dominators
			int best = -1;
			for (int a : doms[b]) { bool ok = 1; for (int c : doms[b]) if (c != a && !count(all(doms[a]), c)) ok = 0; if (ok) best = a; }
			assert(idom[b] == best);
		}
		assert(idom[0] == -1);
	}
	puts("Tests passed!");
}
