// Tests EulerWalk.h (Hierholzer dfs on globals adj/ret/used). Random small
// directed and undirected multigraphs (self-loops allowed, half built from a
// random walk so walks usually exist) are checked against Euler's condition
// computed by brute force (degrees + weak connectivity). Exactly when a walk
// must exist, reversed ret after dfs(start) must be a walk from start using
// every edge once (and end at start when all degrees balance); otherwise the
// output must not be one. Written by Claude (audit).
#include "../utilities/template.h"
#include "../../content/graph/EulerWalk.h"

int main() {
	mt19937 rng(1234);
	auto rnd = [&](int k) { return (int) (rng() % k); };
	F0R(it, 40000) {
		bool undir = rnd(2), big = it % 20 == 0;
		n = rnd(big ? 40 : 8) + 1, m = rnd(big ? 150 : 12);
		vt<pi> edges;
		if (rnd(2)) { // random walk => a trail exists by construction
			int u = rnd(n);
			F0R(i, m) { int v = rnd(n); edges.pb({u, v}); u = v; }
		} else F0R(i, m) edges.pb({rnd(n), rnd(n)});

		// brute force: Euler's condition
		vi in(n), out(n), par(n);
		iota(all(par), 0);
		auto find = [&](int x) { while (par[x] != x) x = par[x]; return x; };
		for (auto [a, b] : edges) out[a]++, in[b]++, par[find(a)] = find(b);
		int comps = 0, odd = 0, over = 0, start = -1;
		F0R(i, n) {
			if (in[i] + out[i] && find(i) == i) comps++;
			int d = undir ? (in[i] + out[i]) % 2 : out[i] - in[i];
			if (d == 1) start = i;
			odd += d != 0, over += abs(d) > 1;
		}
		bool exists = comps <= 1 && odd <= 2 && !over, cycle = exists && !odd;
		if (start < 0 || !odd) do start = rnd(n); while (m && !(in[start] + out[start]));

		// run the header
		adj.assign(n, {});
		F0R(i, m) {
			auto [a, b] = edges[i];
			adj[a].pb({b, i});
			if (undir) adj[b].pb({a, i});
		}
		F0R(i, n) shuffle(all(adj[i]), rng);
		used.assign(m, 0), ret.clear();
		dfs(start);
		reverse(all(ret));

		// validate the walk
		auto valid = [&]() {
			if (size(ret) != m + 1 || ret[0] != start || (cycle && ret.back() != start)) return false;
			map<pi, vi> ids;
			F0R(i, m) {
				auto [a, b] = edges[i];
				ids[{a, b}].pb(i);
				if (undir) ids[{b, a}].pb(i);
			}
			vi seen(m);
			F0R(i, m) {
				bool ok = false;
				for (int e : ids[{ret[i], ret[i + 1]}]) if (!seen[e]) { seen[e] = 1, ok = true; break; }
				if (!ok) return false;
			}
			return true;
		};
		assert(valid() == exists);
	}
	cout << "Tests passed!" << endl;
}
