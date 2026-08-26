// Tests Manacher.h vs brute-force longest even/odd palindrome half-lengths
// around each center; exhaustive over alphabet 3, n <= 12, plus random longer.
// written by Claude (audit)
#include "../utilities/template.h"

#include "../../content/strings/Manacher.h"

void test(const string& s) {
	int n = size(s);
	auto p = manacher(s);
	F0R (i, n) {
		// odd: palindrome centered at i, p[1][i] = half rounded down
		int k = 0;
		while (i - k - 1 >= 0 && i + k + 1 < n && s[i - k - 1] == s[i + k + 1]) k++;
		assert(p[1][i] == k);
		// even: palindrome centered between i-1 and i
		k = 0;
		while (i - k - 1 >= 0 && i + k < n && s[i - k - 1] == s[i + k]) k++;
		assert(p[0][i] == k);
	}
}

template<class F>
void gen(string& s, int at, int alpha, F fun) {
	if (at == size(s)) fun();
	else F0R (i, alpha) { s[at] = (char)('a' + i); gen(s, at + 1, alpha, fun); }
}

int main() {
	F0R (n, 13) { string t(n, 'x'); gen(t, 0, 3, [&]() { test(t); }); }
	srand(7);
	F0R (it, 3000) {
		int n = rand() % 100 + 1, alpha = rand() % 3 + 1;
		string t;
		F0R (i, n) t += (char)('a' + rand() % alpha);
		test(t);
	}
	cout << "Tests passed!" << endl;
}
