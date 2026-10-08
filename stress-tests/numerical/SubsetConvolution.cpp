// Tests SubsetConvolution.h: subset_conv(a, b, n) against the O(3^n) brute force
// c[S] = sum_{T subset S} a[T] b[S\T] mod p, random values in [0, mod), n = 0..10.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/numerical/SubsetConvolution.h"

mt19937 rng(12345);

int main() {
	F0R (n, 11) {
		int M = 1 << n;
		F0R (rep, n <= 6 ? 20 : 3) {
			vl a(M), b(M);
			for (auto& x : a) x = (ll)(rng() % (unsigned)mod);
			for (auto& x : b) x = (ll)(rng() % (unsigned)mod);
			if (rep == 1) fill(all(a), mod - 1), fill(all(b), mod - 1); // max values
			vl want(M);
			F0R (S, M) {
				for (int T = S;; T = (T - 1) & S) {
					want[S] = (want[S] + a[T] * b[S ^ T]) % mod;
					if (!T) break;
				}
			}
			assert(subset_conv(a, b, n) == want);
		}
	}
	cout << "Tests passed!" << endl;
}
