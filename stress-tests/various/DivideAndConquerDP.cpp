// Tests DivideAndConquerDP.h: partition array into g contiguous groups
// (nonneg values, so sum-of-squares cost satisfies QI) vs O(g n^2) DP. Also checks the
// stored argmin k. written by Claude (audit)
#include "../utilities/template.h"

vt<vl> dp;               // dp[ind][k] = prev[k] + cost(k, ind), what f() reads
vt<pair<ll,int>> res;    // res[ind] = (value, k) -- store() below writes this
#define pi(k, v) make_pair(v, k)   // header's store() does res[ind] = pi(k, v)
#include "../../content/various/DivideAndConquerDP.h"
#undef pi

mt19937 rng(7);

int main() {
	F0R (it, 3000) {
		int n = rng() % 12 + 1, g = rng() % n + 1;
		vl a(n), pre(n + 1);
		F0R (i, n) a[i] = rng() % 11, pre[i + 1] = pre[i] + a[i];
		auto cost = [&](int k, int i) { ll s = pre[i] - pre[k]; return s * s; };
		// brute: B[j][i] = best with j groups covering prefix i
		vt<vl> B(g + 1, vl(n + 1, INF)); B[0][0] = 0;
		FOR (j, 1, g + 1) FOR (i, 1, n + 1) F0R (k, i)
			B[j][i] = min(B[j][i], B[j - 1][k] + cost(k, i));
		// D&C: layer by layer, hi(ind)=ind so k in [0, ind)
		vl prev(n + 1, INF); prev[0] = 0;
		FOR (j, 1, g + 1) {
			dp.assign(n + 1, vl(n + 1, INF));
			res.assign(n + 1, {INF, -1});
			FOR (i, 1, n + 1) F0R (k, i) dp[i][k] = prev[k] + cost(k, i);
			DP d; d.solve(1, n + 1);
			FOR (i, 1, n + 1) {
				if (B[j][i] >= INF) { assert(res[i].first >= INF); continue; }
				assert(res[i].first == B[j][i]);
				int k = res[i].second;
				assert(0 <= k && k < i && dp[i][k] == res[i].first);
				// minimal optimal k
				F0R (kk, k) assert(dp[i][kk] > res[i].first);
			}
			F0R (i, n + 1) prev[i] = res[i].first;
		}
	}
	cout << "ok" << endl;
}
