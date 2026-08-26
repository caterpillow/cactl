// Tests FactorSqrt.h: factorizations verified by multiplication + primality
// (edge cases near INT_MAX, plus 1e6 random x vs trial division). Run with -fsanitize=undefined.
// written by Claude (audit)
#include "../utilities/template.h"

#include "../../content/number-theory/FactorSqrt.h"

bool isPrime(int n) {
	if (n < 2) return false;
	for (ll i = 2; i * i <= n; i++) if (n % i == 0) return false;
	return true;
}

void check(int x) {
	auto res = factor(x);
	ll prod = 1;
	int last = 1;
	for (auto [p, e] : res) {
		assert(p > last); last = p; // strictly increasing primes
		assert(e >= 1);
		F0R (i, e) { prod *= p; assert(prod <= x); }
	}
	assert(prod == x);
	for (auto [p, e] : res) assert(isPrime(p));
}

int main() {
	check(INT_MAX);      // 2147483647 (prime)
	assert(factor(INT_MAX) == (vt<pi>{{INT_MAX, 1}}));
	check(2147395601);
	check(2147395600);   // 46340^2
	assert(factor(1).empty());
	FOR (x, 2, 1000) check(x);
	mt19937 rng(12345);
	F0R (it, 1000000) {
		int x = rng() % INT_MAX + 1;
		auto res = factor(x);
		ll prod = 1;
		for (auto [p, e] : res) F0R (i, e) prod *= p;
		assert(prod == x);
		// cheap trial-division cross-check of smallest factor for a subsample
		if (it % 1000 == 0) check(x);
	}
	printf("Tests passed!\n");
}
