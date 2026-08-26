// Tests StaticRangeQuery.h: half-open query(l,r), all widths 1..n, n=1..64,
// with a NON-COMMUTATIVE op (sequence concat via operator+) to verify fold order,
// plus a plain int-sum check. written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/data-structures/StaticRangeQuery.h"

struct Seq { // id is 0 -> Seq(0) must be empty; comb is operator+ (concat)
    vi v;
    Seq() {}
    Seq(int) {}
    Seq(vi v) : v(v) {}
    Seq operator+(const Seq& o) const { Seq r = *this; for (int x : o.v) r.v.pb(x); return r; }
};

int main() {
    FOR (n, 1, 65) {
        vt<Seq> in;
        F0R (i, n) in.pb(Seq(vi{i}));
        RangeQuery<Seq> rq;
        rq.build(all(in));
        F0R (l, n) FOR (r, l + 1, n + 1) {
            vi want;
            FOR (k, l, r) want.pb(k);
            assert(rq.query(l, r).v == want); // order-sensitive
        }
        // int sum sanity
        vi iv(n);
        F0R (i, n) iv[i] = (i * 137 + n * 13) % 1000 - 500;
        RangeQuery<int> rs;
        rs.build(all(iv));
        F0R (l, n) FOR (r, l + 1, n + 1) {
            int s = 0;
            FOR (k, l, r) s += iv[k];
            assert(rs.query(l, r) == s);
        }
    }
    cout << "Tests passed!" << endl;
}
