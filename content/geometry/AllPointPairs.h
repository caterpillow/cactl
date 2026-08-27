/**
 * Author: caterpillow
 * Date: 2025-10-23
 * License: CC0
 * Description: Rotating sweep: visits the points sorted by projection onto
 * every direction, one angle at a time. Needs Point's radial operator<.
 * Pairs whose direction is the same are handled as a group: every maximal
 * block of points on one line (equal projection onto the direction) is
 * reversed, which keeps \texttt{ord} exactly sorted even with collinear
 * points. Example use: min and max triangle area (twice) over all triples.
 * Status: stress-tested (collinear-heavy, vs brute force)
 */

int n; cin >> n;
vt<P> pts(n); // use radial sort
for (auto &[x, y] : pts) cin >> x >> y;

vi ord(n), loc(n);
F0R (i, n) ord[i] = i;
sort(all(ord), [&] (int i, int j) { // sorted for angle -eps
    return pts[i].x != pts[j].x ? pts[i].x < pts[j].x
                                : pts[i].y > pts[j].y; });
F0R (i, n) loc[ord[i]] = i;

// event (x, y) fires when the sweep reaches perp(y - x):
// before it x precedes y in ord, after it y precedes x
vt<tuple<P, int, int>> evs;
F0R (x, n) F0R (y, n) if (x != y)
    evs.pb({(pts[y] - pts[x]).perp(), x, y});
sort(all(evs));
ll mn = INF, mx = -INF;

for (int l = 0, r = 0; l < size(evs); l = r) {
    P d0 = get<0>(evs[l]);
    vi pos; // positions touched by this direction
    while (r < size(evs)) {
        auto [d1, x, y] = evs[r];
        if (d0.cross(d1) || d0.dot(d1) < 0) break;
        pos.pb(loc[x]), pos.pb(loc[y]), r++;
    }
    sort(all(pos)), pos.erase(unique(all(pos)), end(pos));
    for (int s = 0, t; s < size(pos); s = t) { // block = one line
        for (t = s; t + 1 < size(pos) && pos[t + 1] == pos[t] + 1
            && pts[ord[pos[t + 1]]].dot(d0)
                == pts[ord[pos[t]]].dot(d0); t++);
        int lo = pos[s], hi = pos[t++] + 1;
        reverse(ord.begin() + lo, ord.begin() + hi);
        FOR (i, lo, hi) loc[ord[i]] = i;
        // ord[lo - 1], ord[hi]: nearest points on either side of
        // the line; ord[0], ord[n - 1]: the farthest
        if (hi - lo > 2) mn = 0; // three collinear points
        P a = pts[ord[lo]], b = pts[ord[hi - 1]];
        if (hi < n) mn = min(mn, abs(a.cross(b, pts[ord[hi]])));
        if (lo > 0) mn = min(mn, abs(a.cross(b, pts[ord[lo - 1]])));
        mx = max(mx, abs(a.cross(b, pts[ord[0]])));
        mx = max(mx, abs(a.cross(b, pts[ord[n - 1]])));
    }
}
