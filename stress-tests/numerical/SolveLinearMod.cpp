// Tests solve_mod: A x == b whenever rank >= 0, rank matches a brute-force
// rank, -1 iff brute force finds no solution, and rank == m iff the solution
// is unique. Small primes are tested by making `mod` non-const at runtime.
// written by Claude (audit)
#include "../utilities/template.h"

#define const // make `mod` in ModPow.h assignable
#include "../../content/numerical/SolveLinearMod.h"
#undef const

int main() {
	mt19937 rng(2026);
	auto R = [&](ll n) { return (ll) (rng() % (unsigned long long) n); };
	int cnt[3] = {};
	// small primes: exhaustive brute force over all x in Z_p^m
	for (ll p : {2, 3, 5, 7}) {
		mod = p;
		rep(it, 0, 3000) {
			int n = (int) R(5) + 1, m = (int) R(4) + 1;
			vt<vl> A(n, vl(m));
			vl b(n), x;
			for (auto& row : A) for (ll& v : row) v = R(3) ? R(p) : 0;
			if (R(2)) { // consistent construction
				vl y(m);
				for (ll& v : y) v = R(p);
				rep(i, 0, n) rep(j, 0, m) b[i] = (b[i] + A[i][j] * y[j]) % p;
			} else for (ll& v : b) v = R(p);
			ll tot = 1;
			rep(i, 0, m) tot *= p;
			int sols = 0;
			rep(k, 0, tot) {
				ll t = k;
				vl y(m);
				rep(j, 0, m) y[j] = t % p, t /= p;
				bool ok = true;
				rep(i, 0, n) {
					ll sum = 0;
					rep(j, 0, m) sum += A[i][j] * y[j];
					ok &= sum % p == b[i];
				}
				sols += ok;
			}
			int r = solve_mod(A, b, x);
			assert((r == -1) == (sols == 0));
			if (r == -1) { cnt[0]++; continue; }
			assert(size(x) == m);
			rep(i, 0, n) {
				ll sum = 0;
				rep(j, 0, m) assert(0 <= x[j] && x[j] < p), sum += A[i][j] * x[j];
				assert(sum % p == b[i]);
			}
			ll cntsol = 1; // solution count is p^(m - rank)
			rep(i, 0, m - r) cntsol *= p;
			assert(sols == cntsol);
			assert((r == m) == (sols == 1));
			cnt[r == m ? 1 : 2]++;
		}
	}
	assert(cnt[0] > 100 && cnt[1] > 100 && cnt[2] > 100);
	// big prime: verify the returned solution
	mod = 1000000007;
	rep(it, 0, 3000) {
		int n = (int) R(8) + 1, m = (int) R(8) + 1;
		vt<vl> A(n, vl(m));
		vl b(n), y(m), x;
		for (auto& row : A) for (ll& v : row) v = R(4) ? R(mod) : 0;
		for (ll& v : y) v = R(mod);
		rep(i, 0, n) rep(j, 0, m) b[i] = (b[i] + A[i][j] * y[j]) % mod;
		int r = solve_mod(A, b, x);
		assert(r >= 0 && r <= min(n, m));
		rep(i, 0, n) {
			ll sum = 0;
			rep(j, 0, m) sum = (sum + A[i][j] * x[j]) % mod;
			assert(sum == b[i]);
		}
		if (n == m && r == n) assert(x == y); // unique
	}
	cout << "Tests passed!" << endl;
}
