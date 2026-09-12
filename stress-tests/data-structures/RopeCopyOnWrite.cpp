// Copy-on-write / persistence stress test for content/data-structures/Rope.h.
// Runs example(); then a randomized mix of push_back/pop_back, insert (value,
// rope, iterator), erase (range, iterator), substr, replace (value, range),
// operator+, mutable_reference_at and mutable-iterator writes against a
// vector brute force, applied to a main rope and to snapshots (O(1) copies,
// substr of >128-element leaves so nodes are really shared): every version
// must stay intact (persistence / copy-on-write in both directions). Also
// checks const_iterator over an old version kept alive by a copy, mutable
// iterator re-sync by index, Rope<ll> with 0LL, rope(ptr, len).
// Written by Claude (audit).
#include <ext/rope>
#include "../utilities/template.h"
#include "../../content/data-structures/Rope.h"

mt19937 rng(2024);
int rnd(int a, int b) { return (int)uniform_int_distribution<int>(a, b)(rng); }

struct Ver { Rope<int> r; vi b; };

void check(const Ver& v) {
    assert(size(v.r) == size(v.b));
    vi got;
    for (int x : v.r) got.pb(x);
    assert(got == v.b);
    F0R (k, 3) if (!v.b.empty()) { int i = rnd(0, size(v.b) - 1); assert(v.r[i] == v.b[i]); }
}

void test_mix() {
    vt<Ver> vs(1);
    vs.reserve(41); // Ver& v below must survive vs.pb
    F0R (op, 20000) {
        Ver& v = vs[rnd(0, size(vs) - 1)];
        int n = size(v.b), t = rnd(0, 13);
        if (n == 0 && t > 3) t = 0;
        if (n > 3000) t = 4;
        if (t == 0) { int x = rnd(-99, 99); v.r.push_back(x); v.b.pb(x); }
        else if (t == 1) { int i = rnd(0, n), x = rnd(-99, 99); v.r.insert(i, x); v.b.insert(v.b.begin() + i, x); }
        else if (t == 2) { // insert a whole other version
            const Ver& q = vs[rnd(0, size(vs) - 1)];
            int i = rnd(0, n); vi qb = q.b; // q may alias v
            v.r.insert(i, q.r); v.b.insert(v.b.begin() + i, all(qb));
        } else if (t == 3) { // fresh version from a big array (single leaf) or a copy
            Ver w;
            if (rnd(0, 1)) { w.b.resize(rnd(1, 2000)); for (int& x : w.b) x = rnd(-99, 99); w.r = Rope<int>(w.b.data(), size(w.b)); }
            else w = v;
            if (size(vs) < 40) vs.pb(w); else vs[rnd(1, size(vs) - 1)] = w;
        } else if (t == 4) { int i = rnd(0, n - 1), m = rnd(1, 200); m = min(m, n - i); v.r.erase(i, m); v.b.erase(v.b.begin() + i, v.b.begin() + i + m); }
        else if (t == 5) { // substr as a new version (shares nodes when long)
            int i = rnd(0, n - 1), m = rnd(1, n); m = min(m, n - i);
            Ver w{v.r.substr(i, m), vi(v.b.begin() + i, v.b.begin() + i + m)};
            if (size(vs) < 40) vs.pb(w); else vs[rnd(1, size(vs) - 1)] = w;
        } else if (t == 6) { int i = rnd(0, n - 1), x = rnd(-99, 99); v.r.replace(i, x); v.b[i] = x; }
        else if (t == 7) { // replace range by another version
            const Ver& q = vs[rnd(0, size(vs) - 1)];
            int i = rnd(0, n - 1), m = rnd(1, 50); m = min(m, n - i); vi qb = q.b;
            v.r.replace(i, m, q.r);
            v.b.erase(v.b.begin() + i, v.b.begin() + i + m); v.b.insert(v.b.begin() + i, all(qb));
        } else if (t == 8) { const Ver& q = vs[rnd(0, size(vs) - 1)]; vi qb = q.b; if (n + size(qb) > 3000) continue;
            if (rnd(0, 1)) { v.r = q.r + v.r; v.b.insert(v.b.begin(), all(qb)); } else { v.r = v.r + q.r; v.b.insert(v.b.end(), all(qb)); } }
        else if (t == 9) { int i = rnd(0, n - 1), x = rnd(-99, 99); v.r.mutable_reference_at(i) = x; v.b[i] = x; }
        else if (t == 10) { int i = rnd(0, n - 1), x = rnd(-99, 99); *(v.r.mutable_begin() + i) = x; v.b[i] = x; }
        else if (t == 11) { int i = rnd(0, n - 1); auto it = v.r.erase(v.r.mutable_begin() + i); assert((int)it.index() == i); v.b.erase(v.b.begin() + i); }
        else if (t == 12) { int i = rnd(0, n), x = rnd(-99, 99); v.r.insert(v.r.mutable_begin() + i, x); v.b.insert(v.b.begin() + i, x); }
        else { v.r.pop_back(); v.b.pop_back(); }
        check(v);
        check(vs[rnd(0, size(vs) - 1)]);
    }
    for (auto& v : vs) check(v);
}

void test_iterators() {
    Rope<int> r; F0R (i, 500) r.push_back(i);
    Rope<int> keep = r;             // keeps the old tree alive
    auto it = r.begin();            // const_iterator: no refcount, points at the old tree
    r.insert(0, -1); r.erase(100, 50); r.replace(3, 77);
    F0R (i, 500) { assert(*it == i); ++it; }
    auto mit = r.mutable_begin() + 10;
    r.insert(0, -2); r.erase(200, 20);
    assert(*mit == r[10]);          // mutable iterator re-syncs by index
    *mit = 55; assert(r[10] == 55 && keep[10] == 10);
    Rope<int> a, b; F0R (i, 5) a.push_back(i); b = a; b.push_back(9);
    swap(a, b); assert(size(a) == 6 && size(b) == 5 && a[5] == 9);
}

void test_types() {
    Rope<ll> r; r.push_back(1); r.insert(0, 0LL); r.replace(0, 5LL); r.replace(0, 1, Rope<ll>(2, 0LL));
    assert(size(r) == 3 && r[0] == 0 && r[1] == 0 && r[2] == 1);
    vi a = {1, 2, 3}; Rope<int> ra(a.data(), size(a)); Rope<int> sub = ra.substr(1, 2) + ra;
    assert(size(sub) == 5 && sub[0] == 2 && sub[4] == 3);
    ra.erase(0, 1); assert(size(ra) == 2 && ra[0] == 2);   // erase(i, 1), never erase(i)
}

int main() {
    example();
    cout << '\n';
    test_mix();
    test_iterators();
    test_types();
    cout << "Tests passed!" << endl;
}
