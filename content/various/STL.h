/**
 * Author: caterpillow
 * Date: 2023-10-24
 * Description: how to do certain things in stl: custom comparators
 * for containers, and ordering your own structs.
 */
#pragma once

// cmp is a strict order; ties are the same key
auto cmp = [] (T a, T b) { return a < b; };
set<T, decltype(cmp)> s(cmp);
map<T, int, decltype(cmp)> m(cmp);
// pq with a<b is a max-heap; use a>b for a min-heap
priority_queue<T, vt<T>, decltype(cmp)> pq(cmp);

struct S { // comparators for structs: tie = lexicographic
    int a, b;
    bool operator<(const S &o) const {
        return tie(a, b) < tie(o.a, o.b); }
    bool operator==(const S &o) const {
        return tie(a, b) == tie(o.a, o.b); }
};
struct S2 { // C++20: all of < <= > >= == != at once
    int a, b;
    auto operator<=>(const S2 &) const = default;
};
vt<S> w; // ad hoc order without operators:
sort(all(w), [] (S x, S y) { return x.a < y.a; });
