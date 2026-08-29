// Tests graph/Dijkstra.h (radix heap) against a
// plain O(V^2) Dijkstra on random graphs: connected and disconnected, zero
// weights, multi-edges, self loops, and a stress of the modify() path.
// written by Claude (audit)
#include "../utilities/template.h"

#include "../../content/graph/Dijkstra.h"

vl brute(int s) {
	int n = size(adj);
	vl d(n, INF); vt<bool> done(n);
	d[s] = 0;
	F0R (it, n) {
		int u = -1;
		F0R (v, n) if (!done[v] && d[v] < INF && (u < 0 || d[v] < d[u])) u = v;
		if (u < 0) break;
		done[u] = true;
		for (auto [v, w] : adj[u]) d[v] = min(d[v], d[u] + w);
	}
	return d;
}

int main() {
	mt19937_64 rng(12345);
	F0R (iter, 3000) {
		int n = rng() % 9 + 1, m = rng() % 20;
		adj.assign(n, {});
		F0R (i, m) {
			int u = rng() % n, v = rng() % n;
			ll w = rng() % 4 ? rng() % 1000 : 0;   // plenty of zero weights
			adj[u].pb({v, w});
			if (rng() % 2) adj[v].pb({u, w});
		}
		int s = rng() % n;
		assert(dijkstra(s) == brute(s));
	}
	// long path: every edge relaxes an already-queued vertex (modify path)
	int n = 3000;
	adj.assign(n, {});
	F0R (i, n - 1) {
		adj[i].pb({i + 1, 1});
		adj[0].pb({i + 1, (ll) 2 * n});   // heavy edges, later improved
	}
	vl d = dijkstra(0);
	F0R (i, n) assert(d[i] == i);
	cout << "Tests passed!" << endl;
}
