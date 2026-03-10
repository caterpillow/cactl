/**
 * Author: caterpillow
 * Date: 2023-10-24
 * Description: how to do certain things in stl
 */
#pragma once

auto cmp = [&] (T a, T b) { return a < b; }
set<T, decltype(cmp)> s(cmp);
map<T, int, decltype(cmp)> m(cmp);
priority_queue<T, vt<T>, decltype(cmp)> pq(cmp); // max
