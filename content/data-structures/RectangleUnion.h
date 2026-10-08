/**
 * Author: Claude
 * Date: 2026-10-06
 * License: CC0
 * Source: folklore (sweep line + counting segment tree)
 * Description: Area of the union of rectangles $[x_1,x_2) \times
 * [y_1,y_2)$ with $y \in [0, \texttt{MY})$ (compress first).
 * Usage: vt<array<int, 4>> r = {{x1, y1, x2, y2}, ...};
 *  ll A = rect_union(r);
 * Time: O(N \log N)
 * Status: stress-tested
 */
#pragma once

const int MY = 1 << 20; // y range, power of two
int cnt[2 * MY], len[2 * MY];
void upd(int a, int b, int v, int i = 1,
        int l = 0, int r = MY) {
    if (b <= l || r <= a) return;
    if (a <= l && r <= b) cnt[i] += v;
    else {
        int m = (l + r) / 2;
        upd(a, b, v, 2 * i, l, m), upd(a, b, v, 2 * i + 1, m, r);
    }
    if (cnt[i]) len[i] = r - l;
    else len[i] = r - l == 1 ? 0 : len[2 * i] + len[2 * i + 1];
}
ll rect_union(vt<array<int, 4>> R) {
    vt<array<int, 4>> ev; // x, +-1, y1, y2
    for (auto [x1, y1, x2, y2] : R) if (x1 < x2 && y1 < y2)
        ev.pb({x1, 1, y1, y2}), ev.pb({x2, -1, y1, y2});
    sort(all(ev));
    ll res = 0;
    F0R (i, size(ev)) {
        if (i) res += (ll) len[1] * (ev[i][0] - ev[i - 1][0]);
        upd(ev[i][2], ev[i][3], ev[i][1]);
    }
    return res;
}
