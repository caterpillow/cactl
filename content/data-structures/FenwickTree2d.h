/**
 * Author: Simon Lindholm
 * Date: 2017-05-11
 * License: CC0
 * Source: folklore
 * Description: Computes sums a[i,j] for all i<I, j<J, and increases single elements a[i,j].
 *  Requires that the elements to be updated are known in advance (call fake\_update() before init()).
 * Time: $O(\log^2 N)$. (Use persistent segment trees for $O(\log N)$.)
 * Status: stress-tested
 */
#pragma once

#include "FenwickTree.h"

struct FT2 {
	vt<vi> ys; vt<FT> ft;
	FT2(int limx) : ys(limx) {}
	void fake_update(int x, int y) {
		for (; x < size(ys); x |= x + 1) ys[x].push_back(y);
	}
	void init() {
		for (vi &v : ys) sort(all(v)), ft.emplace_back(size(v));
	}
	int ind(int x, int y) {
		return lower_bound(all(ys[x]), y) - ys[x].begin(); 
	}
	void update(int x, int y, ll dif) {
		for (; x < size(ys); x |= x + 1)
			ft[x].update(ind(x, y), dif);
	}
	ll query(int x, int y) {
		ll sum = 0;
		for (; x; x &= x - 1)
			sum += ft[x-1].query(ind(x-1, y));
		return sum;
	}
};
