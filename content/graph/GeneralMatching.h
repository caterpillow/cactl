/**
 * Author: Simon Lindholm
 * Date: 2016-12-09
 * License: CC0
 * Source: http://www.mimuw.edu.pl/~mucha/pub/mucha_sankowski_focs04.pdf
 * Description: Matching for general graphs.
 * Fails with probability $N / mod$.
 * Time: O(N^3)
 * Status: not very well tested
 */
#pragma once

#include "../numerical/MatrixInverse-mod.h"

vt<pi> generalMatching(int N, vt<pi>& ed) {
	vt<vl> mat(N, vl(N)), A;
	for (pi pa : ed) {
		int a = pa.f, b = pa.s, r = rand() % mod;
		mat[a][b] = r, mat[b][a] = (mod - r) % mod;
	}

	int r = matInv(A = mat), M = 2*N - r, fi, fj;
	assert(r % 2 == 0);

	if (M != N) do {
		mat.resize(M, vl(M));
		F0R (i, N) {
			mat[i].resize(M);
			FOR (j, N, M) {
				int r = rand() % mod;
				mat[i][j] = r, mat[j][i] = (mod - r) % mod;
			}
		}
	} while (matInv(A = mat) != M);

	vi has(M, 1); vt<pi> ret;
	F0R (it, M/2) {
		F0R (i, M) if (has[i])
			FOR (j, i+1, M) if (A[i][j] && mat[i][j]) {
				fi = i; fj = j; goto done;
		} assert(0); done:
		if (fj < N) ret.pb({fi, fj});
		has[fi] = has[fj] = 0;
		F0R (sw, 2) {
			ll a = mpow(A[fi][fj]);
			F0R (i, M) if (has[i] && A[i][fj]) {
				ll b = A[i][fj] * a % mod;
				F0R (j, M) A[i][j] = (A[i][j] - A[fi][j] * b) % mod;
			}
			swap(fi,fj);
		}
	}
	return ret;
}
