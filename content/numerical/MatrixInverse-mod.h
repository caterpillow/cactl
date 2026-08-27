/**
 * Author: Simon Lindholm
 * Date: 2016-12-08
 * Source: The regular matrix inverse code
 * Description: Invert matrix $A$ modulo a prime.
 * Returns rank; result is stored in $A$ unless singular (rank < n).
 * For prime powers, repeatedly set $A^{-1} = A^{-1} (2I - AA^{-1})\  (\text{mod }p^k)$ where $A^{-1}$ starts as
 * the inverse of A mod p, and k is doubled in each step.
 * Time: O(n^3)
 * Status: Slightly tested
 */
#pragma once

#include "../number-theory/ModPow.h"

int matInv(vt<vl>& A) {
	int n = size(A); vi col(n);
	vt<vl> tmp(n, vl(n));
	F0R (i, n) tmp[i][i] = 1, col[i] = i;

	F0R (i, n) {
		int r = i, c = i;
		FOR (j, i, n) FOR (k, i, n) if (A[j][k]) {
			r = j; c = k; goto found;
		}
		return i;
found:
		A[i].swap(A[r]); tmp[i].swap(tmp[r]);
		F0R (j, n)
			swap(A[j][i], A[j][c]), swap(tmp[j][i], tmp[j][c]);
		swap(col[i], col[c]);
		ll v = mpow(A[i][i]);
		FOR (j, i + 1, n) {
			ll f = A[j][i] * v % mod;
			A[j][i] = 0;
			FOR (k, i + 1, n) A[j][k] = (A[j][k] - f*A[i][k]) % mod;
			F0R (k, n) tmp[j][k] = (tmp[j][k] - f*tmp[i][k]) % mod;
		}
		FOR (j, i + 1, n) A[i][j] = A[i][j] * v % mod;
		F0R (j, n) tmp[i][j] = tmp[i][j] * v % mod;
		A[i][i] = 1;
	}

	for (int i = n-1; i > 0; --i) F0R (j, i) {
		ll v = A[j][i];
		F0R (k, n) tmp[j][k] = (tmp[j][k] - v*tmp[i][k]) % mod;
	}

	F0R (i, n) F0R (j, n)
		A[col[i]][col[j]] = tmp[i][j] % mod + (tmp[i][j] < 0)*mod;
	return n;
}
