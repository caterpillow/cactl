// Tests duval (Lyndon factorization): factors are Lyndon words, non-increasing,
// and match a brute-force greedy factorization (longest Lyndon prefix).
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/strings/Lyndon.h"

bool isLyndon(const string& w) {
	int n = sz(w);
	rep(i,1,n) if (!(w < w.substr(i))) return false;
	return n > 0;
}

vi brute(const string& s) {
	int n = sz(s); vi st;
	for (int i = 0; i < n;) {
		int best = i + 1;
		rep(j,i+1,n+1) if (isLyndon(s.substr(i, j - i))) best = j;
		st.push_back(i); i = best;
	}
	return st;
}

int main() {
	mt19937 rng(12345);
	rep(it,0,200000) {
		int alpha = (int)(rng() % 4) + 1, n = (int)(rng() % 61);
		string s(n, 'a');
		for (char& c : s) c = (char)('a' + (int)(rng() % (unsigned)alpha));
		vi st = duval(s);
		vi b = brute(s);
		assert(st == b);
		// direct verification
		assert((n == 0) == st.empty());
		string prev;
		rep(i,0,sz(st)) {
			int l = st[i], r = i + 1 < sz(st) ? st[i+1] : n;
			assert(l < r);
			string w = s.substr(l, r - l);
			assert(isLyndon(w));
			if (i) assert(prev >= w);
			prev = w;
		}
		if (!st.empty()) assert(st[0] == 0);
	}
	cout << "Tests passed!" << endl;
}
