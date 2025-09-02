/**
 * Author: Ulf Lundstrom
 * Date: 2009-04-08
 * License: CC0
 * Source:
 * Description: Returns the center of mass for a polygon.
 * Time: O(n)
 * Status: Tested
 */
#pragma once

#include "Point.h"

typedef Point<db> P;
P polygon_center(const vector<P>& v) {
	P res(0, 0); db a = 0;
	for (int i = 0, j = size(v) - 1; i < size(v); j = i++) {
		res = res + (v[i] + v[j]) * v[j].cross(v[i]);
		a += v[j].cross(v[i]);
	}
	return res / a / 3;
}
