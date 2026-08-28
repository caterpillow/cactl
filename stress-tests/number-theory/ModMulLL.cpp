// Tests ModMulLL.h: mmul(a,b,M) vs __int128 for edge-biased 0<=a,b<=M<=7.2e18,
// mmul(a,a,M) with a slightly above M (Factor.h relies on this), mpow vs an
// int128 brute force, and a few known values. Written by Claude (audit).
#include "../utilities/template.h"

#include "../../content/number-theory/ModMulLL.h"

ull ref(ull a, ull b, ull m) { return (ull)((__uint128_t)a * b % m); }

void check(ull a, ull b, ull c, ull got, ull want) {
	if (got != want) {
		cout << "FAIL " << a << ' ' << b << ' ' << c << ": got " << got << " want " << want << endl;
		exit(1);
	}
}

int main() {
	const ull LIM = 7268172458553106874ULL; // floor((sqrt(177) - 7) / 16 * 2^64), the proven bound
	mt19937_64 rng(1);
	uniform_int_distribution<ull> uni(1, LIM), tiny(0, LIM / 10000);
	uniform_int_distribution<ull> sq(0, (ull)(sqrtl((long double)LIM) / 2));

	for (int i = 0, it = 0; it < 5'000'000; i++) { // a, b, c biased toward the boundaries
		ull c = i & 1 ? LIM - tiny(rng) : uni(rng);
		ull a = i & 2 ? c - tiny(rng) : i & 4 ? (1ULL << 62) - tiny(rng) : uni(rng);
		ull b = i & 8 ? c - tiny(rng) : uni(rng);
		if (a > c || b > c) continue;
		it++;
		check(a, b, c, mmul(a, b, c), ref(a, b, c));
	}
	F0R(i, 2'000'000) { // squaring with c <= a <= c + sqrt(c)/2, as Factor.h's rho does
		ull c = i & 1 ? LIM - tiny(rng) : uni(rng);
		ull a = c + sq(rng);
		check(a, a, c, mmul(a, a, c), ref(a, a, c));
	}
	F0R(i, 200'000) { // mpow vs int128 square-and-multiply
		ull m = uni(rng), b = uni(rng) % m, e = i & 1 ? rng() % 1000 : rng();
		ull want = 1, x = b;
		for (ull k = e; k; k >>= 1, x = ref(x, x, m)) if (k & 1) want = ref(want, x, m);
		check(b, e, m, mpow(b, e, m), want);
	}
	check(3, 4, 5, mmul(3, 4, 5), 2);
	check(2, 10, 1000, mpow(2, 10, 1000), 24);
	check(5, 0, 7, mpow(5, 0, 7), 1);
	check(1, 1, 1, mmul(1, 1, 1), 0);
	check(LIM, LIM, LIM, mmul(LIM, LIM, LIM), 0);
	cout << "Tests passed!" << endl;
}
