/**
 * Author: Victor Lecomte, chilli
 * Date: 2019-04-26
 * License: CC0
 * Source: https://vlecomte.github.io/cp-geo.pdf
 * Description: Returns true iff p lies on the line segment from s to e.
 * Use \texttt{(seg\_dist(s,e,p) < epsilon)} instead when using Point<double>.
 * Status:
 */
#pragma once

#include "Point.h"

template<class P> bool on_segment(P s, P e, P p) {
    return p.cross(s, e) == 0 && (s - p).dot(e - p) <= 0;
}