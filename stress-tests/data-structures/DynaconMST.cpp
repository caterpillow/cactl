// Tests DynaconMST.h (offline dynacon via IncrementalMST, weight = -deletion
// time): random add/delete/query events on small graphs, answers checked
// against a per-query DSU rebuild. Includes re-adding deleted edges.
// written by Claude (audit)
#include "../utilities/template.h"

#include "../../content/data-structures/DynaconMST.h"

int main() {
	mt19937 rng(7);
	F0R (it, 3000) {
		int n = rng() % 7 + 1, Q = rng() % 60 + 1;
		vt<array<int, 3>> ev(Q);
		set<pi> alive;
		F0R (t, Q) {
			int u = rng() % n, v = rng() % n, ty = rng() % 3;
			if (rng() % 2) swap(u, v); // unnormalised input
			pi e = {min(u, v), max(u, v)};
			if (ty == 0 && u != v && !alive.count(e)) alive.insert(e);
			else if (ty == 1 && !alive.empty()) {
				e = *next(alive.begin(), rng() % size(alive));
				u = e.f, v = e.s; alive.erase(e);
			} else ty = 2;
			ev[t] = {ty, u, v};
		}
		vt<bool> got = dynacon(n, ev), want;
		set<pi> cur;
		F0R (t, Q) {
			auto [ty, u, v] = ev[t];
			pi e = {min(u, v), max(u, v)};
			if (ty == 0) cur.insert(e);
			if (ty == 1) cur.erase(e);
			if (ty == 2) {
				vi p(n); iota(all(p), 0);
				function<int(int)> f = [&](int x) { return p[x] == x ? x : p[x] = f(p[x]); };
				for (auto [a, b] : cur) p[f(a)] = f(b);
				want.pb(f(u) == f(v));
			}
		}
		assert(got == want);
	}
	cout << "Tests passed!" << endl;
}
