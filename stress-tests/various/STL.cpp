// Tests various/STL.h: with a concrete T, set/map iterate ascending and
// priority_queue with the "a < b" comparator is a max-heap (the "// max"
// comment is correct); struct S orders lexicographically via tie. Checked vs sort().
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
		// struct S: operator< is lexicographic (a, b); operator== consistent
		F0R (i, n) w.pb({(int)(rng() % 5), (int)(rng() % 5)});
		vt<S> ws = w; sort(all(ws)); set<S> ss(all(w));
		F0R (i, size(ws) - 1) assert(!(ws[i + 1] < ws[i]) && tie(ws[i].a, ws[i].b) <= tie(ws[i + 1].a, ws[i + 1].b));
		assert(size(ss) <= size(w)); // set<S> compiles via operator<
		F0R (i, size(ws)) F0R (j, size(ws))
			assert((ws[i] == ws[j]) == (tie(ws[i].a, ws[i].b) == tie(ws[j].a, ws[j].b)));
	}
	cout << "Tests passed!" << endl;
}
