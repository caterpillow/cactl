/**
 * Author: Johan Sannemo
 * Date: 2016-12-15
 * License: CC0
 * Description: pi[x] computes the length of the longest prefix of s that ends at x,
 * other than s[0...x] itself (abacaba -> 0010123).
 * Can be used to find all occurrences of a string.
 * Time: O(n)
 * Status: Tested on kattis:stringmatching
 */
#pragma once

vi pfun(const string& s) { // renamed: pi is a template alias
	vi p(size(s));
	FOR (i, 1, size(s)) {
		int g = p[i - 1];
		while (g && s[i] != s[g]) g = p[g - 1];
		p[i] = g + (s[i] == s[g]);
	}
	return p;
}

vi match(const string &s, const string &pat) {
	vi p = pfun(pat + '\0' + s), res;
	FOR (i, size(p) - size(s), size(p))
		if (p[i] == size(pat)) res.push_back(i - 2 * size(pat));
	return res;
}
