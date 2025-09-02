/**
 * Author: Oleksandr Bacherikov, chilli
 * Date: 2019-05-05
 * License: Boost Software License
 * Source: https://codeforces.com/blog/entry/48868
 * Description: Returns the two points with max distance on a convex hull (ccw,
 * no duplicate/collinear points).
 * Status: stress-tested, tested on kattis:roberthood
 * Time: O(n)
 */
#pragma once
#include "Point.h"

typedef Point<ll> P;
array<P, 2> hull_diameter(vector<P> s) {
	int n = size(s), j = n < 2 ? 0 : 1;
	pair<ll, array<P, 2>> res({0, {s[0], s[0]}});
	F0R (i, j)
		for (;; j = (j + 1) % n) {
			res = max(res, {(s[i] - s[j]).dist2(), {s[i], s[j]}});
			if ((s[(j + 1) % n] - s[j]).cross(s[i + 1] - s[i]) >= 0)
				break;
		}
	return res.second;
}
