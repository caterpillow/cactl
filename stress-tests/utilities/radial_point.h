// Point<ll> with the radial comparator that Point.h keeps commented out,
// for templates whose header says "radial comparator". Include it before
// such a template: it is an explicit specialisation, so it has to come
// before anything instantiates Point<ll>. written by Claude (audit)
#pragma once
#include "../../content/geometry/Point.h"

template<> struct Point<ll> {
    using P = Point<ll>;
    ll x, y;
    P operator+(P p) const { return {x + p.x, y + p.y}; }
    P operator-(P p) const { return {x - p.x, y - p.y}; }
    P operator*(ll z) const { return {x * z, y * z}; }
    ll dot(P p) const { return x * p.x + y * p.y; }
    ll cross(P p) const { return x * p.y - y * p.x; }
    P perp() const { return {-y, x}; }
    int half() const { return y < 0 || (y == 0 && x < 0); }
    bool operator<(P p) const {
        return make_pair(half(), (ll) 0) < make_pair(p.half(), cross(p));
    }
    bool operator==(P p) const { return tie(x, y) == tie(p.x, p.y); }
};
