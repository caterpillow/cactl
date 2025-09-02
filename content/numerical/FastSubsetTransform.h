/**
 * Author: Lucian Bicsi
 * Date: 2015-06-25
 * License: GNU Free Documentation License 1.2
 * Source: csacademy
 * Description: Transform to a basis with fast convolutions of the form
 * $\displaystyle c[z] = \sum\nolimits_{z = x \oplus y} a[x] \cdot b[y]$,
 * where $\oplus$ is one of AND, OR, XOR. The size of $a$ must be a power of two.
 * Replace with long longs and do operations under mod if needed.
 * Time: O(N \log N)
 * Status: stress-tested
 */
#pragma once

using pi = pair<int, int>;
void FST(vi &a, bool inv) {
	for (int n = size(a), step = 1; step < n; step *= 2) {
		for (int i = 0; i < n; i += 2 * step) FOR (j, i, i + step) {
			int &u = a[j], &v = a[j + step]; tie(u, v) =
				inv ? pi(v - u, u) : pi(v, u + v); // AND
				// inv ? pi(v, u - v) : pi(u + v, u); // OR /// include-line
				// pi(u + v, u - v);                   // XOR /// include-line
		}
	}
	// if (inv) for (int& x : a) x /= size(a); // XOR only /// include-line
}
vi conv(vi a, vi b) {
	FST(a, 0); FST(b, 0);
	F0R (i, size(a)) a[i] *= b[i];
	FST(a, 1); return a;
}