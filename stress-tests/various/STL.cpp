// Tests various/STL.h: with a concrete T, set/map iterate ascending and
// priority_queue with the "a < b" comparator is a max-heap (the "// max"
// comment is correct). Checked vs sort() on random inputs.
// written by Claude (audit)
#include "../utilities/template.h"

using T = pair<int, int>;
int main() {
	mt19937 rng(123);
	F0R (it, 2000) {
		#include "../../content/various/STL.h"
		int n = rng() % 30;
		vt<T> v; F0R (i, n) v.pb({(int)(rng() % 5), (int)(rng() % 5)});
		for (T x : v) { s.insert(x); m[x]++; pq.push(x); }
		vt<T> sv(all(s)), ref = v; sort(all(ref)); ref.erase(unique(all(ref)), ref.end());
		assert(sv == ref);
		vt<T> mk; for (auto& [k, c] : m) { mk.pb(k); assert(c == count(all(v), k)); }
		assert(mk == ref);
		vt<T> pv; while (!pq.empty()) pv.pb(pq.top()), pq.pop();
		vt<T> rd = v; sort(all(rd), greater<T>());
		assert(pv == rd);
	}
	cout << "Tests passed!" << endl;
}
