/**
 * Author: Johan Sannemo, pajenegod
 * Date: 2015-02-06
 * License: CC0
 * Source: Folklore
 * Description: Range Minimum Queries on an array. Returns
 * min(V[a], V[a + 1], ... V[b - 1]) in constant time.
 * Usage:
 *  RMQ rmq(values);
 *  rmq.query(inclusive, exclusive);
 * Time: $O(|V| \log |V| + Q)$
 * Status: stress-tested
 */
#pragma once

template<class T>
struct RMQ {
	vt<vt<T>> jmp;
	RMQ(const vt<T>& v) : jmp(1, v) {
		for (int pw = 1, k = 1; pw * 2 <= size(v); pw *= 2, ++k) {
			jmp.emplace_back(size(v) - pw * 2 + 1);
			F0R (j, size(jmp[k]))
				jmp[k][j] = min(jmp[k - 1][j], jmp[k - 1][j + pw]);
		}
	}
	T query(int a, int b) {
		assert(a < b); // or return inf if a == b
		int d = 31 - __builtin_clz(b - a);
		return min(jmp[d][a], jmp[d][b - (1 << d)]);
	}
};
