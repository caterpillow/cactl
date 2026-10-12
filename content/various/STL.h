/**
 * Author: caterpillow
 * Date: 2023-10-24
 * Description: how to do certain things in stl: custom comparators
 * for containers, ordering your own structs, moving containers.
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

// move(x): O(1) steal of x's buffer, x left empty (reusable).
// Plain push_back(x) / v = x copy in O(size).
vt<vi> st; vi cur;
st.push_back(move(cur)); // cur is now empty
vi top = move(st.back()); st.pop_back(); // pop: move out first
// pq.top() is const&: const_cast to move (fine in libstdc++)
vi t = move(const_cast<vi&>(pq.top())); pq.pop();
swap(a, b); // O(1) for any container except array (and pbds)
if (size(a) < size(b)) swap(a, b); // small-to-large merge
// emplace(args...) builds the element in place from ctor args
vt<pi> e; e.emplace_back(1, 2); // same as push_back({1, 2})
pq.emplace(d, u); q.emplace(u); s.emplace(x); // all have it
// emplace_back(x) == push_back(x): copies unless move(x).
// Don't move: return v (auto-moved), const objects (copy).
// CPU seconds since program start (e.g. stop at 0.9*limit)

double secs = (double)clock() / CLOCKS_PER_SEC;
