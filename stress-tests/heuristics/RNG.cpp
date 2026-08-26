// Tests RNG.h: Mersenne twister output must be bit-identical to std::mt19937
// seeded with the same integer, for 100k draws over several seeds; next_double in [0,1).
// written by Claude (audit)
#include "../utilities/template.h"

#include "../../content/heuristics/RNG.h"

int main() {
	for (unsigned seed : {1u, 2u, 5489u, 123456789u, 4294967295u}) {
		RNG r((int) seed);
		mt19937 std_mt(seed);
		F0R (i, 100000) {
			unsigned a = r.rand(), b = std_mt();
			assert(a == b);
		}
	}
	RNG r(42);
	F0R (i, 1000000) {
		double d = r.next_double();
		assert(d >= 0.0 && d < 1.0);
	}
	F0R (i, 100000) {
		int v = r.next(3, 17);
		assert(v >= 3 && v < 17);
	}
	// splitmix64 sanity: deterministic, matches reference first value for x=1234
	assert(splitmix64(0) == 0xe220a8397b1dcdafULL);
	printf("Tests passed!\n");
}
