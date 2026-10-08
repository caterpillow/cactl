// Tests BitsetLCS.h: lcs() vs the O(nm) DP on random strings over small and
// large alphabets, lengths 0..200 (incl. 63/64/65, 127/128/129), bytes >= 128.
// written by Claude (audit)
#include "../utilities/template.h"

#include "../../content/strings/BitsetLCS.h"

int brute(const string& a, const string& b) {
	int n = size(a), m = size(b);
	vt<vi> dp(n + 1, vi(m + 1));
	F0R (i, n) F0R (j, m)
		dp[i + 1][j + 1] = a[i] == b[j] ? dp[i][j] + 1 : max(dp[i][j + 1], dp[i + 1][j]);
	return dp[n][m];
}

int main() {
	mt19937 rng(12345);
	auto rnd = [&](int lo, int hi) { return (int) (rng() % (unsigned) (hi - lo + 1)) + lo; };
	vi special = {0, 1, 2, 63, 64, 65, 127, 128, 129, 191, 192, 193, 200};
	auto len = [&]() { return rnd(0, 1) ? special[rnd(0, size(special) - 1)] : rnd(0, 200); };
	auto gen = [&](int n, int alpha, int base) {
		string s(n, ' ');
		for (char& c : s) c = (char) (base + rnd(0, alpha - 1));
		return s;
	};
	int cnt = 0;
	for (int it = 0; it < 20000; it++) {
		int alpha = vi{1, 2, 3, 4, 26, 256}[rnd(0, 5)];
		int base = alpha == 256 ? 0 : rnd(0, 1) ? 'a' : 256 - alpha;
		string a = gen(len(), alpha, base), b = gen(len(), alpha, base);
		if (rnd(0, 3) == 0) b = a; // identical
		else if (rnd(0, 3) == 0) { // subsequence of a
			b.clear();
			for (char c : a) if (rnd(0, 1)) b += c;
		}
		assert(lcs(a, b) == brute(a, b));
		cnt++;
	}
	// all-equal strings: carries propagate across every word
	F0R (n, 201) F0R (m, 201) if (n % 7 == 0 || m % 5 == 0)
		assert(lcs(string(n, '\xff'), string(m, '\xff')) == min(n, m)), cnt++;
	cerr << cnt << " cases\n";
	cout << "Tests passed!\n";
}
