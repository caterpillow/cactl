/**
 * Author: Simon Lindholm
 * Date: 2015-03-15
 * License: CC0
 * Source: own work
 * Description: Various self-explanatory methods for string hashing.
 * Use on Codeforces, which lacks 64-bit support and where solutions can be hacked.
 * Status: stress-tested
 */
#pragma once

using ull = uint64_t;
static int C; // initialized below

// Arithmetic mod two primes and 2^32 simultaneously.
// "using H = uint64_t;" instead if Thue-Morse does not apply.
template<int M, class B>
struct A {
	int x; B b; A(int x=0) : x(x), b(x) {}
	A(int x, B b) : x(x), b(b) {}
	A operator+(A o){int y = x+o.x; return{y - (y>=M)*M, b+o.b};}
	A operator-(A o){int y = x-o.x; return{y + (y< 0)*M, b-o.b};}
	A operator*(A o) { return {(int)(1LL*x*o.x % M), b*o.b}; }
	explicit operator ull() { return x ^ (ull) b << 21; }
	bool operator==(A o) const { return (ull)*this == (ull)o; }
	bool operator<(A o) const { return (ull)*this < (ull)o; }
};
using H = A<1000000007, A<1000000009, unsigned>>;

struct HashInterval {
	vt<H> ha, pw;
	HashInterval(string& str) : ha(size(str)+1), pw(ha) {
		pw[0] = 1;
		F0R (i, size(str))
			ha[i+1] = ha[i] * C + str[i],
			pw[i+1] = pw[i] * C;
	}
	H hashInterval(int a, int b) { // hash [a, b)
		return ha[b] - ha[a] * pw[b - a];
	}
};

vt<H> getHashes(string& str, int length) {
	if (size(str) < length) return {};
	H h = 0, pw = 1;
	F0R (i, length)
		h = h * C + str[i], pw = pw * C;
	vt<H> ret = {h};
	FOR (i, length, size(str)) {
		ret.pb(h = h * C + str[i] - pw * str[i-length]);
	}
	return ret;
}

H hashString(string& s){H h{}; for(char c:s) h=h*C+c;return h;}

#include <sys/time.h>
int main() {
	timeval tp;
	gettimeofday(&tp, 0);
	C = (int)tp.tv_usec; // (less than modulo)
	assert((ull)(H(1)*2+1-3) == 0);
	// ...
}
