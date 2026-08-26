// Tests IntDeterminant.h (the REAL header) with mod = 1e9+7: random small
// matrices vs exact determinant computed via long-double Gaussian elimination
// (values kept small enough to round exactly), reduced mod p.
// written by Claude (audit)
#include "../utilities/template.h"
ll mod = 1e9 + 7; // global consumed by the header
#include "../../content/numerical/IntDeterminant.h"

mt19937 rng(555);

ll floatDet(vt<vt<long double>> a) {
	int n = size(a); long double res = 1;
	F0R (i, n) {
		int b = i;
		FOR (j, i + 1, n) if (fabsl(a[j][i]) > fabsl(a[b][i])) b = j;
		if (i != b) swap(a[i], a[b]), res = -res;
		res *= a[i][i];
		if (res == 0) return 0;
		FOR (j, i + 1, n) {
			long double v = a[j][i] / a[i][i];
			if (v != 0) FOR (k, i + 1, n) a[j][k] -= v * a[i][k];
		}
	}
	return llroundl(res);
}

int main() {
	F0R (it, 50000) {
		int n = rng() % 7; // 0..6; |det| <= 6! * 20^6 ~ 4.6e10, exact in ldb
		vt<vl> m(n, vl(n));
		vt<vt<long double>> fm(n, vt<long double>(n));
		F0R (i, n) F0R (j, n) {
			ll v = (ll)(rng() % 41) - 20;
			if (rng() % 5 == 0) v = 0;
			m[i][j] = v; fm[i][j] = (long double)v;
		}
		ll want = floatDet(fm) % mod;
		if (want < 0) want += mod;
		ll got = det(m);
		assert(got == want);
	}
	// also negative entries reduced into (-mod, mod), larger magnitude, n<=3
	F0R (it, 20000) {
		int n = rng() % 4;
		vt<vl> m(n, vl(n));
		vt<vt<long double>> fm(n, vt<long double>(n));
		F0R (i, n) F0R (j, n) {
			ll v = (ll)(rng() % 20001) - 10000;
			m[i][j] = v; fm[i][j] = (long double)v;
		}
		ll want = floatDet(fm) % mod;
		if (want < 0) want += mod;
		assert(det(m) == want);
	}
	cout << "Tests passed!" << endl;
}
