/**
 * Author: Ulf Lundstrom
 * Date: 2009-04-11
 * License: CC0
 * Source: http://en.wikipedia.org/wiki/Circumcircle
 * Description:\\
\begin{minipage}{75mm}
The circumcircle of a triangle is the circle intersecting all three vertices. \texttt{cc\_radius} returns the radius of the circle going through points A, B and C and \texttt{cc\_center} returns the center of the same circle. A, B, C must not be collinear.
\end{minipage}
\begin{minipage}{15mm}
\vspace{-2mm}
\includegraphics[width=\textwidth]{content/geometry/circumcircle}
\end{minipage}
 * Status: tested
 */
#pragma once

#include "Point.h"

using P = Point<db>;
db cc_radius(const P& A, const P& B, const P& C) {
    return (B - A).dist() * (C - B).dist() * (A - C).dist() /
            abs((B - A).cross(C - A)) / 2;
}
P cc_center(const P& A, const P& B, const P& C) {
    P b = C - A, c = B - A;
    return A + (b * c.dist2() - c * b.dist2()).perp() / b.cross(c) / 2;
}
