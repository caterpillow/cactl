/**
 * Author: caterpillow
 * Date: 2025-10-21
 * Description: Prefix/suffix min/max over keys with insertions, a replacement for sparse segtrees.
 * Usage: RangeQuery<less<>, less_equal<>> m; // suffix min
 *  m.ins(1e9, INF); // sentinel past all keys
 *  m.ins(k, v); m.query(k); // min v over keys >= k
 *  prefix: greater<>, sentinel ins(-1e9, INF); max: greater_equal<>, sentinel -INF.
 * Time: O(\log N)
 */
#pragma once

template<class dir, class cmp>
struct RangeQuery {
    map<int, ll, dir> data;
    void ins(int k, ll v) {
        if (auto it = data.lower_bound(k); it != data.end() && cmp{}(it->s, v)) return;
        auto it = data.insert_or_assign(k, v).f;
        while (it != data.begin() && cmp{}(v, prev(it)->s)) data.erase(prev(it));
    }
    ll query(int k) { // inclusive
        return data.lower_bound(k)->s;
    }
};