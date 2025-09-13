/**
 * Author: caterpillow
 * Date: 2025-09-09
 * License: CC0
 * Source: idk
 * Description: Generic segment tree for associative operations. \texttt{id} is for the identity object.
 * Time: O(\log N)
 * Status: true
 */
#pragma once

struct Segtree {
	int n;
	vt<T> seg;
	void init(int _n) {
		for (n = 1; n < _n; n *= 2);
		seg.resize(2 * n, id);
	}
	void upd(int i, T v) {
		seg[i += n] = v;
		while (i /= 2) seg[i] = seg[2 * i] + seg[2 * i + 1];
	}
	T query(int l, int r) { 
		T lhs = id, rhs = id;
		for (l += n, r += n; l < r; l /= 2, r /= 2) {
			if (l & 1)) lhs = lhs + seg[l++];
			if (r & 1) rhs = seg[--r] + rhs;
		}
		return lhs + rhs;
	}
};