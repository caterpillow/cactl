/**
 * Author: caterpillow
 * Date: 2025-09-18
 * License: CC0
 * Source: https://cp-algorithms.com/algebra/prime-sieve-linear.html
 * Description: Linear sieve implementation. Around 0.5s for 1e8. Also computes smallest prime divisor for each number in lp. 
 * Status: Tested
 */
#pragma once

const int mx = 1e8; // ~0.5s
vi lp(mx + 1), primes;

FOR (i, 2, mx + 1) {
	if (lp[i] == 0) lp[i] = i, primes.pb(i);
	for (int j = 0; i * primes[j] <= mx; j++) {
		lp[i * primes[j]] = primes[j];
		// store i to avoid division when factoring
		if (primes[j] == lp[i]) break;
	}
} 

auto factor = [&] (int x) {
	vt<pair<int, int>> res;
	while (x > 1) {
		if (!size(res) || res.back().first != lp[x]) 
			res.pb({lp[x], 0});
		res.back().second++;
		x /= lp[x]; // can avoid division by storing extra array
	}
	return res;
};