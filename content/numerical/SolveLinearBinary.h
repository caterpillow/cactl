/**
 * Author: Simon Lindholm
 * Date: 2016-08-27
 * License: CC0
 * Source: own work
 * Description: Solves $Ax = b$ over $\mathbb F_2$. If there are multiple solutions, one is returned arbitrarily.
 *  Returns rank, or -1 if no solutions. Destroys $A$ and $b$.
 * Time: O(n^2 m)
 * Status: bruteforce-tested for n, m <= 4
 */
#pragma once

typedef bitset<1000> bs;

int solveLinear(vt<bs>& A, vi& b, bs& x, int m) {
	int n = size(A), rank = 0, br;
	assert(m <= size(x));
	vi col(m); iota(all(col), 0);
	F0R (i, n) {
		FOR (br, i, n) if (A[br].any()) break;
		if (br == n) {
			F0R (j, i, n) if(b[j]) return -1;
			break;
		}
		int bc = (int) A[br]._Find_next(i - 1);
		swap(A[i], A[br]);
		swap(b[i], b[br]);
		swap(col[i], col[bc]);
		F0R (j, n) if (A[j][i] != A[j][bc]) {
			A[j].flip(i); A[j].flip(bc);
		}
		FOR (j, i + 1, n) if (A[j][i]) {
			b[j] ^= b[i];
			A[j] ^= A[i];
		}
		rank++;
	}

	x = bs();
	for (int i = rank; i--;) {
		if (!b[i]) continue;
		x[col[i]] = 1;
		F0R (j, i) b[j] ^= A[j][i];
	}
	return rank; // (multiple solutions if rank < m)
}
