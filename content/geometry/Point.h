/**
 * Author: caterpillow
 * Date: 2025-08-24
 * License: CC0
 * Source: modified the original kactl 
 * Description: Class to handle points in the plane.
 * 	T can be e.g. double or long long. (Avoid int.)
 * Status: there's no way this is broken
 */
#pragma once

template <class T> int sgn(T x) { return (x > 0) - (x < 0); }
template<class T>
struct Point {
    using P = Point<T>;
    T x, y;
    #define op1(o) P operator o (P p) const { return {x o p.x, y o p.y}; }
    op1(+) op1(-)
    #define op2(o) P operator o (T z) const { return {x o z, y o z}; }
    op2(*) op2(/)
    T dot(P p) const { return x * p.x + y * p.y; }
    T cross(P p) const { return x * p.y - y * p.x; }
    #define op3(o) T o (P a, P b) const { return (a - *this). o (b - *this); }
    op3(dot) op3(cross)
    #define op4(o) bool operator o (P p) const { return tie(x, y) o tie(p.x, p.y); }
    op4(<) op4(==)
    T dist2() const { return x * x + y * y; }
    db dist() const { return sqrtl((db) dist2()); }
    // angle to x-axis in interval [-pi, pi]
    db angle() const { return atan2l(y, x); }
    P unit() const { return *this / dist(); } 
    P perp() const { return {-y, x}; } // rotate 90 degrees left
    P normal() const { return perp().unit(); }
    P rotate(db a) const {
        return P(x * cos(a) - y * sin(a), x * sin(a) + y * cos(a)); 
    }
    friend ostream& operator<<(ostream& os, P p) {
        return os << "(" << p.x << ", " << p.y << ") "; 
    }
};