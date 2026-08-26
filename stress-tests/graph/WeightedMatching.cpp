// Tests WeightedMatching.h (hungarian, ll costs): vs permutation brute force
// n<=m<=5 with small and +-1e9 costs; explicit 3x3 all-1e9 overflow regression.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../utilities/random.h"
#include "../../content/graph/WeightedMatching.h"

ll brute(vt<vl>& a) {
	int n = size(a), m = size(a[0]);
	vi perm(m); iota(all(perm), 0);
	ll best = INF;
	do {
		ll cur = 0;
		F0R (i, n) cur += a[i][perm[i]];
		best = min(best, cur);
	} while (next_permutation(all(perm)));
	return best;
}

int main() {
	srand(1234);
	assert(hungarian({}).first == 0);
	{ // old int-overflow case
		vt<vl> a(3, vl(3, (ll) 1e9));
		assert(hungarian(a).first == (ll) 3e9);
	}
	F0R (it, 50000) {
		int n = randIncl(1, 5), m = randIncl(n, 5);
		bool big = it & 1;
		vt<vl> a(n, vl(m));
		F0R (i, n) F0R (j, m)
			a[i][j] = big ? randIncl((int64_t) -1000000000, (int64_t) 1000000000)
			              : randIncl(-10, 10);
		auto [cost, match] = hungarian(a);
		assert(cost == brute(a));
		ll sum = 0; vi used(m);
		F0R (i, n) {
			assert(0 <= match[i] && match[i] < m && !used[match[i]]++);
			sum += a[i][match[i]];
		}
		assert(sum == cost);
	}
	cout << "Tests passed!" << endl;
}
