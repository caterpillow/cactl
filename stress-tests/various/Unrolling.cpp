// Tests Unrolling.h: the unrolled loop (alignment prologue, 4x body, tail)
// must visit exactly the indices [from, to) in order, for random bounds
// including from >= to and bounds not divisible by 4.
// written by Claude (audit)
#include "../utilities/template.h"

ll run(const vl& a, int from, int to, vi& visit) {
	ll sum = 0;
#define F { sum += a[i]; visit.push_back(i); ++i; }
	int i = from;
	while (i & 3 && i < to) F // for alignment, if needed
	while (i + 4 <= to) { F F F F }
	while (i < to) F
#undef F
	return sum;
}

int main() {
	mt19937 rng(12345);
	rep(it,0,20000) {
		int n = rng() % 40;
		vl a(n); for (auto& x : a) x = (ll)(rng() % 2000001) - 1000000;
		int from = rng() % (n + 1), to = rng() % (n + 1);
		vi visit, exp;
		ll s = run(a, from, to, visit);
		ll es = 0; rep(j,from,to) { es += a[j]; exp.push_back(j); }
		assert(s == es && visit == exp);
	}
	cout << "Tests passed!" << endl;
}
