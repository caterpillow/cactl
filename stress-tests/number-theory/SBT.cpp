// Tests SBT.h cont_frac: for random p>=0, q>0 the continued fraction
// reconstructs p/q exactly, terms are valid, and convergents match p/q.
// written by Claude (audit)
#include "../utilities/template.h"

#include "../../content/number-theory/SBT.h"

int main() {
	mt19937 rng(999);
	F0R (it, 500000) {
		int p = rng() % 1000000, q = rng() % 1000000 + 1;
		if (it < 100) { p = it % 10; q = it / 10 + 1; } // small edge cases incl p=0
		vi a = cont_frac(p, q);
		assert(!a.empty());
		// terms after the first must be positive; last term > 1 unless whole cf is [x]
		FOR (i, 1, size(a)) assert(a[i] >= 1);
		if (size(a) > 1) assert(a.back() > 1);
		// reconstruct back-to-front: value = a[i] + 1/value
		ll np = a.back(), nq = 1;
		ROF (i, 0, size(a) - 1) { swap(np, nq); np += (ll) a[i] * nq; }
		int g = __gcd(p, q);
		assert(np == p / g && nq == q / g);
		// convergents h_k/k_k recurrence ends at p/q reduced
		ll h1 = 1, h0 = 0, k1 = 0, k0 = 1;
		for (int ai : a) { ll h = ai * h1 + h0, k = ai * k1 + k0; h0 = h1; h1 = h; k0 = k1; k1 = k; }
		assert(h1 == p / g && k1 == q / g);
	}
	printf("Tests passed!\n");
}
