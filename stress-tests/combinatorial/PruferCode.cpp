// Tests PruferCode.h: decode(code) is a tree, encode(decode(code)) == code,
// and the degree property (v appears deg(v)-1 times). Written by Claude (audit).
#include "../utilities/template.h"
#include "../../content/combinatorial/PruferCode.h"
mt19937 rng(9);
int main() {
	F0R (it, 20000) {
		int n = rng() % 10 + 2; vi code(n - 2);
		for (int& v : code) v = rng() % n;
		vt<pi> ed = prufer_decode(code);
		assert(size(ed) == n - 1);
		vt<vi> adj(n); vi p(n); iota(all(p), 0);
		auto f = [&](auto& f, int x) -> int { return p[x] == x ? x : p[x] = f(f, p[x]); };
		for (auto [u, v] : ed) {
			assert(u != v && f(f, u) != f(f, v));
			p[f(f, u)] = f(f, v); adj[u].pb(v), adj[v].pb(u);
		}
		assert(prufer_encode(adj) == code);
		vi deg(n), cnt(n);
		for (auto [u, v] : ed) deg[u]++, deg[v]++;
		for (int v : code) cnt[v]++;
		F0R (v, n) assert(cnt[v] == deg[v] - 1);
	}
	puts("Tests passed!");
}
