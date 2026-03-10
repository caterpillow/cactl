/**
 * Author: David Rydh, Per Austrin
 * Date: 2003-03-16
 * Description:
 */
#pragma once

struct Poly {
	vt<db> a;
	db operator()(double x) const {
		double val = 0;
		for (int i = size(a); i--;) (val *= x) += a[i];
		return val;
	}
	void diff() {
		FOR (i, 1, size(a)) a[i - 1] = i * a[i];
		a.pop_back();
	}
	void divroot(double x0) {
		db b = a.back(), c; a.back() = 0;
		for (int i = size(a) - 1; i--;) c = a[i], a[i] = a[i + 1] * x0 + b, b = c;
		a.pop_back();
	}
};
