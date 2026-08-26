// Tests LIS.h: lis_dp[i] vs O(n^2) DP (length of strict LIS ending at i),
// lis_construct indices form a valid maximal strictly-increasing subsequence
// (maximality checked by bitmask brute), incl. empty and fully-increasing
// inputs (old out-of-bounds case). written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/various/LIS.h"

mt19937 rng(999);

void check(const vi& v) {
	int n = size(v);
	// brute LIS length + per-index DP
	vi len(n, 1);
	int best = 0;
	F0R (i, n) {
		F0R (j, i) if (v[j] < v[i]) len[i] = max(len[i], len[j] + 1);
		best = max(best, len[i]);
	}
	vi dp = lis_dp(v);
	assert(size(dp) == n);
	F0R (i, n) assert(dp[i] == len[i]);
	vi idx = lis_construct(v);
	assert(size(idx) == best);
	F0R (k, size(idx) - 1) {
		assert(idx[k] < idx[k + 1]);
		assert(v[idx[k]] < v[idx[k + 1]]);
	}
	F0R (k, size(idx)) assert(0 <= idx[k] && idx[k] < n);
}

int main() {
	check({}); // empty
	FOR (n, 1, 12) { // fully increasing (old OOB when LIS length == n)
		vi v(n); iota(all(v), 0);
		check(v);
	}
	F0R (it, 200000) {
		int n = rng() % 9; // 0..8
		vi v(n);
		for (int& x : v) x = (int)(rng() % 5) - 2;
		if (it % 10 == 0) sort(all(v)); // sorted-ish, incl. strictly increasing
		if (it % 20 == 0) { v.assign(n, 0); iota(all(v), (int)(rng() % 3)); }
		check(v);
	}
	cout << "Tests passed!" << endl;
}
