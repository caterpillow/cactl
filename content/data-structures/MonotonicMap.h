/**
 * Author: caterpillow
 * Date: 2025-10-21
 * Description: A data structure for performing prefix/suffix min/max queries with insertions. Useful replacement for sparse segtrees. 
 * Time: O(\log N)
 */
#pragma once

// dir = less<> for suffix queries, greater<> for prefix
// cmp = less_equal<> for min, greater_equal<> for max
template<class dir, class cmp> 
struct RangeQuery {
    map<int, ll, dir> data; 
    void ins(int k, ll v) {
        if (auto it = data.lower_bound(k); it != data.end() && cmp{}(it->s, v)) return;
        auto it = data.insert_or_assign(k, v).f;
        while (it != data.begin() && cmp{}(v, prev(it)->s)) data.erase(prev(it));
    }
    ll query(int k) { // inclusive: careful UB, insert infinity value
        return data.lower_bound(k)->s;
    }
};