// Tests FastEratosthenes.h: eratosthenes() and the isPrime bitset against a
// plain O(n log log n) sieve at LIM, plus a dynamic-LIM copy of the same
// segmented code against brute force for every LIM in [2, 3000] and a few
// larger ones. Written by Claude (audit).
#include "../utilities/template.h"
#include "../../content/number-theory/FastEratosthenes.h"

// primes < n, brute force
vi brute(int n) {
	vt<bool> comp(max(n, 2));
	vi res;
	FOR (i, 2, n) if (!comp[i]) {
		res.pb(i);
		for (ll j = (ll)i * i; j < n; j += i) comp[(int)j] = 1;
	}
	return res;
}

// same code as the header, LIM as a runtime parameter
vi dyn(int LIM) {
	const int S = (int) round(sqrt(LIM)), R = LIM / 2;
	vi pr = {2}, sieve(S + 1); pr.reserve(int(LIM / log(LIM) * 1.1));
	vt<pi> cp;
	for (int i = 3; i <= S; i += 2) if (!sieve[i]) {
		cp.pb({i, i * i / 2});
		for (int j = i * i; j <= S; j += 2 * i) sieve[j] = 1;
	}
	for (int L = 1; L <= R; L += S) {
		vt<bool> block(S);
		for (auto &[p, idx] : cp)
			for (int i = idx; i < S + L; idx = (i += p)) block[i - L] = 1;
		F0R (i, min(S, R - L))
			if (!block[i]) pr.pb((L + i) * 2 + 1);
	}
	return pr;
}

int main() {
	vi pr = eratosthenes(), br = brute(LIM);
	assert(pr == br);
	assert(size(pr) == 78498);
	F0R (i, LIM) assert(isPrime[i] == binary_search(all(br), i));

	FOR (lim, 3, 3001) assert(dyn(lim) == brute(lim));
	for (int lim : {12345, 65537, 99991, 100000, 1 << 17, 999983, 2000003})
		assert(dyn(lim) == brute(lim));
	cout << "Tests passed!" << endl;
}
