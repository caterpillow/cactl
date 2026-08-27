/**
 * Author: Johan Sannemo
 * Date: 2014-11-28
 * License: CC0
 * Source: Folklore
 * Description: Calculate submatrix sums quickly, given upper-left and lower-right corners (half-open).
 * Usage:
 * SubMatrix<int> m(matrix);
 * m.sum(0, 0, 2, 2); // top left 4 elements
 * Time: O(N^2 + Q)
 * Status: Tested on Kattis
 */
#pragma once

template<class T>
struct SubMatrix {
	vt<vt<T>> p;
	SubMatrix(vt<vt<T>>& v) {
		int R = size(v), C = size(v[0]);
		p.assign(R+1, vt<T>(C+1));
		F0R (r, R) F0R (c, C)
			p[r+1][c+1] = v[r][c] + p[r][c+1] + p[r+1][c] - p[r][c];
	}
	T sum(int u, int l, int d, int r) {
		return p[d][r] - p[d][l] - p[u][r] + p[u][l];
	}
};
