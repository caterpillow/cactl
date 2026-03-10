/**
 * Author: Mattias de Zalenski, Fredrik Niemelä, Per Austrin, Simon Lindholm
 * Date: 2002-09-26
 * Source: Max Bennedich
 * Description: Computes $\displaystyle \binom{k_1 + \dots + k_n}{k_1, k_2, \dots, k_n} = \frac{(\sum k_i)!}{k_1!k_2!...k_n!}$.
 * Status: Tested on kattis:lexicography
 */
#pragma once

ll multinomial(vi& v) {
	ll c = 1, m = v.empty() ? 1 : v[0];
	FOR (i, 1, size(v)) F0R (j, v[i]) c = c * ++m / (j + 1);
	return c;
}
