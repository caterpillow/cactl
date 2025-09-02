/**
 * Author: Stanford
 * Source: Stanford Notebook
 * License: MIT
 * Description: Solves a general linear maximization problem: maximize $c^T x$ subject to $Ax \le b$, $x \ge 0$.
 * Returns -inf if there is no solution, inf if there are arbitrarily good solutions, or the maximum value of $c^T x$ otherwise.
 * The input vector is set to an optimal $x$ (or in the unbounded case, an arbitrary solution fulfilling the constraints).
 * Numerical stability is not guaranteed. For better performance, define variables such that $x = 0$ is viable.
 * Usage:
 * vvd A = {{1,-1}, {-1,1}, {-1,-2}};
 * vd b = {1,1,-4}, c = {-1,-1}, x;
 * T val = LPSolver(A, b, c).solve(x);
 * Time: O(NM * \#pivots), where a pivot may be e.g. an edge relaxation. O(2^n) in the general case.
 * Status: seems to work?
 */
#pragma once

#define mp make_pair
using T = db;
using vd = vt<db>;
using vvd = vt<vd>;
const db eps = 1e-9;
const int inf = 1e9;

#define ltj(X) if (s == -1 || mp(X[j], N[j]) < mp(X[s], N[s])) s = j
struct LPSolver {
	int m, n;
	vt<int> N, B; 
	vvd D; 
	LPSolver(const vvd& A, const vd& b, const vd& c) :
	  m(size(b)), n(size(c)), N(n + 1), B(m), D(m + 2, vd(n + 2)) {
		F0R(i, m) F0R(j, n) D[i][j] = A[i][j]; 
		F0R(i, m) B[i] = n+i, D[i][n] = -1, D[i][n + 1] = b[i];
		F0R(j, n) N[j] = j, D[m][j] = -c[j];
		N[n] = -1; D[m + 1][n] = 1; 
	} 
	void pivot(int r, int s) { 
		T inv = 1 / D[r][s]; 
		F0R(i, m + 2) if (i != r && abs(D[i][s]) > eps) {
			T binv = D[i][s]*inv;
			F0R (j, n + 2) if (j != s) D[i][j] -= D[r][j]*binv;
			D[i][s] = -binv;
		}
		D[r][s] = 1; F0R(j, n + 2) D[r][j] *= inv; // scale r-th row
		swap(B[r],N[s]);
	}
	bool simplex(int phase) {
		int x = m + phase - 1;
		while (1) { 
			int s = -1; F0R (j, n + 1) if (N[j] != -phase) ltj(D[x]); 
			if (D[x][s] >= -eps) return 1; 
			int r = -1;
			F0R (i, m) {
				if (D[i][s] <= eps) continue;
				if (r == -1 || mp(D[i][n + 1] / D[i][s], B[i])
							 < mp(D[r][n + 1] / D[r][s], B[r])) r = i; 
			} 
			if (r == -1) return 0; 
			pivot(r, s);
		}
	}
	T solve(vd &x) { 
		int r = 0; FOR (i, 1, m) if (D[i][n + 1] < D[r][n + 1]) r = i;
		if (D[r][n + 1] < -eps) { 
			pivot(r,n); 
			assert(simplex(2)); 
			if (D[m + 1][n + 1] < -eps) return -inf;
			F0R (i, m) if (B[i] == -1) { 
				int s = 0; FOR (j, 1, n + 1) ltj(D[i]); 
				pivot(i, s);
			}
		}
		bool ok = simplex(1); x = vd(n);
		F0R (i, m) if (B[i] < n) x[B[i]] = D[i][n + 1];
		return ok ? D[m][n + 1] : inf;
	}
};
