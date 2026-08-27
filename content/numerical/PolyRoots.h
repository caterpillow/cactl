/**
 * Author: Per Austrin
 * Date: 2004-02-08
 * License: CC0
 * Description: Finds the distinct real roots in $[xmin,xmax]$.
 * With double, use well-scaled inputs and simple roots separated
 * by $\gg 10^{-8}\max(1,|x|)$. Multiple roots are unsafe.
 * Usage: poly_roots({{2,-3,1}},-1e9,1e9)
 * Time: $O(n^3)$, where $n$ is the degree.
 */
#pragma once

#include "Polynomial.h"

vt<db> poly_roots(Poly p, db xmin, db xmax) {
    assert(xmin <= xmax);
    while (!p.a.empty() && p.a.back() == 0) p.a.pop_back();
    if (size(p.a) <= 1) return {};
    if (size(p.a) == 2) {
        db x = -p.a[0] / p.a[1];
        return xmin <= x && x <= xmax ? vt<db>{x} : vt<db>{};
    }
    Poly der = p;
    der.diff();
    auto dr = poly_roots(der, xmin, xmax);
    sort(all(dr));
    vt<db> xs{xmin};
    for (db x : dr) if (xmin < x && x < xmax) xs.pb(x);
    if (xmin < xmax) xs.pb(xmax);
    vt<db> ret;
    F0R (i, size(xs)) {
        db fl = p(xs[i]);
        if (fl == 0) ret.pb(xs[i]);
        if (i + 1 == size(xs)) break;
        db fh = p(xs[i + 1]);
        if (fl == 0 || fh == 0 || (fl < 0) == (fh < 0))
            continue;
        db l = xs[i], h = xs[i + 1];
        F0R (it, 60) {
            db m = l / 2 + h / 2;
            if (m == l || m == h) break;
            db fm = p(m);
            if (fm == 0) { l = h = m; break; }
            if ((fm < 0) == (fl < 0)) l = m, fl = fm;
            else h = m;
        }
        ret.pb(l / 2 + h / 2);
    }
    return ret;
}
