/**
 * Author: caterpillow
 * Date: 2025-10-23
 * License: CC0
 * Description: considers points sorted with respect to every direction
 * Status: i think it works
 */

int n; cin >> n;
vt<P> pts(n);
for (auto &[x, y] : pts) cin >> x >> y;

vi ord(n), loc(n);
F0R (i, n) ord[i] = i;
sort(all(ord), [&] (int i, int j) { return pts[i].x < pts[j].x; });
F0R (i, n) loc[ord[i]] = i;

// when the dot vector reaches P, x should be greater than y 
vt<tuple<P, int, int>> evs;
F0R (x, n) {
    F0R (y, n) {
        if (x == y) continue;
        // consider only adding .half() == 0 for better runtime
        evs.pb({(pts[y] - pts[x]).perp(), x, y});
    }
}
sort(all(evs));
ll mn = INF, mx = -INF;

for (auto [dir, x, y] : evs) {
    int &i = loc[x], &j = loc[y];
    if (i < j) swap(ord[i], ord[j]), swap(i, j);
    // x is "greater" than y with respect to dir (ie. i > j) and x is on the left
    if (i - j > 1) mn = 0;
    if (i + 1 < n) mn = min(mn, pts[x].cross(pts[y], pts[ord[i + 1]]));
    if (j - 1 >= 0) mn = min(mn, -pts[x].cross(pts[y], pts[ord[j - 1]]));

    mx = max(mx, abs(pts[x].cross(pts[y], pts[ord[0]])));
    mx = max(mx, abs(pts[x].cross(pts[y], pts[ord[n - 1]])));
}