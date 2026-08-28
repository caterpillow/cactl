// Tests SegmentTree.h: point upd + range query [l, r) vs brute on a product
// monoid (max, D3 composition) so operand order, identity, empty ranges and
// non-power-of-2 sizes are all checked. written by Claude (audit)
#include "../utilities/template.h"

// Cayley table of D3 (non-commutative), 0 = identity
const int lut[6][6] = {
    {0, 1, 2, 3, 4, 5}, {1, 0, 4, 5, 2, 3}, {2, 5, 0, 4, 3, 1},
    {3, 4, 5, 0, 1, 2}, {4, 3, 1, 2, 5, 0}, {5, 2, 3, 1, 0, 4}};
struct T {
    int mx, g;
    T operator+(const T& o) const { return {max(mx, o.mx), lut[g][o.g]}; }
    bool operator==(const T& o) const { return mx == o.mx && g == o.g; }
};
const T id{-inf, 0};

#include "../../content/data-structures/SegmentTree.h"

int main() {
    // sanity: lut is an associative, non-commutative monoid with unit 0
    bool comm = true;
    F0R (a, 6) F0R (b, 6) {
        assert(lut[0][a] == a && lut[a][0] == a);
        if (lut[a][b] != lut[b][a]) comm = false;
        F0R (c, 6) assert(lut[lut[a][b]][c] == lut[a][lut[b][c]]);
    }
    assert(!comm);

    mt19937 rng(7);
    auto rnd = [&](int k) { return (int) (rng() % k); };
    { Segtree tr; tr.init(0); assert(tr.query(0, 0) == id); }
    FOR (n, 1, 70) {
        Segtree tr;
        tr.init(n);
        vt<T> v(n, id);
        F0R (it, 3000) {
            if (rnd(2)) {
                int i = rnd(n);
                v[i] = {rnd(1000) - 500, rnd(6)};
                tr.upd(i, v[i]);
            } else {
                int l = rnd(n + 1), r = rnd(n + 1);
                if (l > r) swap(l, r);
                T b = id;
                FOR (k, l, r) b = b + v[k];
                assert(tr.query(l, r) == b);
            }
        }
    }
    cout << "Tests passed!" << endl;
}
