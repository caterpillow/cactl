// Tests MinimumVertexCover.h + DFSMatching.h + hopcroftKarp.h: all matching
// sizes agree with an independent Kuhn brute; cover touches every edge and
// |cover| == matching size. written by Claude (audit)
#include "../utilities/template.h"
#include "../utilities/random.h"
#include "../../content/graph/MinimumVertexCover.h" // includes DFSMatching.h
#include "../../content/graph/hopcroftKarp.h"

// independent Kuhn brute force
bool aug(int v, vt<vi>& g, vi& mt, vi& us) {
	for (int j : g[v]) if (!us[j]) {
		us[j] = 1;
		if (mt[j] == -1 || aug(mt[j], g, mt, us)) { mt[j] = v; return 1; }
	}
	return 0;
}
int kuhn(vt<vi>& g, int m) {
	vi mt(m, -1), us; int r = 0;
	F0R (i, size(g)) us.assign(m, 0), r += aug(i, g, mt, us);
	return r;
}

void run(int it, int N, int M, int density) {
	F0R (t, it) {
		int n = randIncl(0, N), m = randIncl(0, M);
		vt<vi> g(n);
		F0R (i, n) F0R (j, m) if (randRange(100) < density) g[i].pb(j);
		int want = kuhn(g, m);
		vi btoa(m, -1);
		assert(dfsMatching(g, btoa) == want);
		Matching hk(m); int s2 = 0;
		F0R (i, n) s2 += hk.add(g[i]);
		assert(s2 == want);
		vi cov = cover(g, n, m);
		assert(size(cov) == want);
		vi inc(n + m);
		for (int x : cov) { assert(0 <= x && x < n + m); assert(!inc[x]++); }
		F0R (i, n) for (int j : g[i]) assert(inc[i] || inc[n + j]);
	}
}

int main() {
	srand(1234);
	run(30000, 10, 10, 25);
	run(5000, 8, 8, 70);
	run(200, 80, 80, 5);
	run(100, 60, 100, 40);
	cout << "Tests passed!" << endl;
}
