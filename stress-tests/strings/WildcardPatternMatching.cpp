// Tests WildcardPatternMatching.h vs brute-force wildcard matching:
// alphabet <= 4 plus wildcard '*', N <= 60, M <= N, 30k random cases.
// Asserts no false negatives; counts false positives (expected 0).
// written by Claude (audit)
#include "../utilities/template.h"

#include "../../content/strings/WildcardPatternMatching.h"

int main() {
	mt19937 mrng(9001);
	ll fp = 0, cases = 0;
	F0R (it, 30000) {
		int n = mrng() % 60 + 1, m = mrng() % n + 1;
		int alpha = mrng() % 4 + 1;
		auto rc = [&]() { int x = mrng() % (alpha + 1);
			return x == alpha ? '*' : (char)('a' + x); };
		string t, p;
		F0R (i, n) t += rc();
		F0R (i, m) p += rc();
		string res = wildcard_pattern_matching(t, p, '*');
		assert(size(res) == n - m + 1);
		F0R (i, n - m + 1) {
			bool ok = true;
			F0R (j, m) if (t[i + j] != '*' && p[j] != '*' && t[i + j] != p[j]) ok = false;
			if (ok) assert(res[i] == '1'); // never a false negative
			else if (res[i] == '1') fp++;
			cases++;
		}
	}
	printf("positions checked: %lld, false positives: %lld\n", cases, fp);
	assert(fp == 0);
	cout << "Tests passed!" << endl;
}
