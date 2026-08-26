// Tests Hashing.h: hash_string == hash_interval(0,n) == get_hashes(s,n)[0],
// all interval/rolling/reverse hashes vs independent recomputation, no collisions.
// written by Claude (audit)
#include "../utilities/template.h"

#include "../../content/strings/Hashing.h"

H brute(const string& t) { H h = 0; for (char c : t) h = h * C + c + 1; return h; }

int main() {
	assert((H(1)*2+1-3).get() == 0);
	srand(42);
	F0R (it, 20000) {
		int n = rand() % 12;
		int alpha = rand() % 10 + 1;
		string s;
		F0R (i, n) s += (char)('a' + rand() % alpha);
		HashInterval hi(s);
		set<string> strs;
		set<ull> hashes;

		// hash_string agreement
		assert(hash_string(s).get() == hi.hash_interval(0, n).get());
		if (n) assert(get_hashes(s, n)[0].get() == hash_string(s).get());

		for (int i = 0; i <= n; i++) for (int j = i; j <= n; j++) {
			string sub = s.substr(i, j - i);
			ull h = brute(sub).get();
			assert(hi.hash_interval(i, j).get() == h);
			string rsub(sub.rbegin(), sub.rend());
			assert(hi.rhash_interval(i, j).get() == brute(rsub).get());
			hashes.insert(h);
			strs.insert(sub);
		}

		// get_hashes (all rolling hashes of each length)
		FOR (le, 1, n + 1) {
			auto ve = get_hashes(s, le);
			assert(size(ve) == n - le + 1);
			F0R (i, n - le + 1)
				assert(ve[i].get() == hi.hash_interval(i, i + le).get());
		}
		assert(size(strs) == size(hashes)); // no collisions
	}
	cout << "Tests passed!" << endl;
}
