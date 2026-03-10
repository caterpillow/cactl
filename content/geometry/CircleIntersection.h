/**
 * Author: Simon Lindholm
 * Date: 2015-09-01
 * License: CC0
 * Description: Computes the pair of points at which two circles intersect.
 * Returns false in case of no intersection.
 * Status: stress-tested
 */
#pragma once

#include "Point.h"

using P = Point<db>;
bool circle_inter(P a, P b, db r1, db r2, pair<P, P> *out) {
	if (a == b) { assert(r1 != r2); return false; }
	P vec = b - a;
	db d2 = vec.dist2(), sum = r1 + r2, dif = r1 - r2,
	       p = (d2 + r1 * r1 - r2 * r2) / (d2 * 2), h2 = r1 * r1 - p * p * d2;
	if (sum * sum < d2 || dif * dif > d2) return false;
	P mid = a + vec * p, per = vec.perp() * sqrt(fmax(0, h2) / d2);
	*out = {mid + per, mid - per};
	return true;
}
