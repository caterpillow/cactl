/**
 * Author: Simon Lindholm
 * Date: 2015-02-04
 * License: CC0
 * Source: Johan Sannemo
 * Description: Poor man's optimization for unimodal functions.
 * Minimizes \texttt{f} over the plane, returns (min, argmin).
 * Usage: auto [v, p] = hillClimb({0, 0}, f); // f(P) -> db
 * Time: 87301 calls to \texttt{f}
 * Status: used with great success
 */
#pragma once

using P = array<db, 2>;

template<class F> pair<db, P> hillClimb(P start, F f) {
    pair<db, P> cur(f(start), start);
    for (db jmp = 1e9; jmp > 1e-20; jmp /= 2) {
        F0R (j, 100) FOR (dx, -1, 2) FOR (dy, -1, 2) {
            P p = cur.second;
            p[0] += dx * jmp;
            p[1] += dy * jmp;
            db v = f(p); if (v < cur.f) cur = {v, p};
        }
    }
    return cur;
}
