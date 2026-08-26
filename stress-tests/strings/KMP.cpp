// Tests KMP.h: pfun() exhaustively vs brute prefix function on small alphabets
// (~2.2M strings), plus match() vs brute substring search on random cases.
// written by Claude (audit)
#include "../utilities/template.h"

#include "../../content/strings/KMP.h"

template<class F>
void gen(string& s, int at, int alpha, F fun) {
	if (at == size(s)) fun();
	else F0R (i, alpha) { s[at] = (char)('a' + i); gen(s, at + 1, alpha, fun); }
}

void test(const string& s) {
	vi p = pfun(s);
	F0R (i, size(s)) {
		int maxlen = -1;
		F0R (len, i + 1) {
			F0R (j, len) if (s[j] != s[i + 1 - len + j]) goto fail;
			maxlen = len;
fail:;
		}
		assert(maxlen == p[i]);
	}
}

int main() {
	F0R (n, 13) { string t(n, 'x'); gen(t, 0, 3, [&]() { test(t); }); }
	F0R (n, 11) { string t(n, 'x'); gen(t, 0, 4, [&]() { test(t); }); }

	// match() vs brute
	srand(1234);
	F0R (it, 20000) {
		int n = rand() % 30, m = rand() % 8 + 1, alpha = rand() % 3 + 1;
		string t, pat;
		F0R (i, n) t += (char)('a' + rand() % alpha);
		F0R (i, m) pat += (char)('a' + rand() % alpha);
		vi res = match(t, pat), exp;
		F0R (i, n - m + 1) if (t.substr(i, m) == pat) exp.pb(i);
		assert(res == exp);
	}
	cout << "Tests passed!" << endl;
}
