/**
 * Author: caterpillow
 * Date: 2025-09-18
 * License: CC0
 * Source: own
 * Description: Easy factorisation. Returns pairs of {prime factor, multiplicity}
 * Time: $O(\sqrt{N})$.
 * Status: no way this doesn't work
 */
#pragma once

vt<pair<int, int>> factor(int x) {
	if (x == 1) return {};
	vt<pair<int, int>> res;
	for (int i = 2; i * i <= x; i++) {
		if (x % i == 0) {
			res.pb({i, 0});
			while (x % i == 0) {
				res.back().second++;
				x /= i;
			}
		}
	}
	if (x > 1) res.pb({x, 1});
	return res;
}
