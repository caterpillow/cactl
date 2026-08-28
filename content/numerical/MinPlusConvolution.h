/**
 * Author: brunodccarvalho
 * Date: 2026-07-06
 * Description: Min-plus convolution with one convex or concave input.
 *  SMAWK handles arbitrary/convex; the border algorithm handles arbitrary/concave.
 *  Here, convex = smiley and concave = frowny.
 *  \texttt{min\_smawk(f,r,c)} returns a minimizing column for each row of the
 *  matrix f. Totally monotone means every submatrix's leftmost row-minimum
 *  indices are nondecreasing. The matrix is queried through f, not stored.
 * Time: $O(N+M)$ convex, $O(N\log M+M)$ concave
 */
#pragma once

// Compute row-minima indices for totally monotone f(x,y). O(N + M)
template<class Fn>
vi min_smawk_rec(Fn& f, const vi &row, vi col) {
    int n = size(row);
    if (!n) return {};
    vi red;
    for (int c : col) {
        while (size(red) &&
            f(row[size(red) - 1], c) < f(row[size(red) - 1], red.back()))
            red.pop_back();
        if (size(red) < n) red.pb(c);
    } // <hash>
    col = move(red);

    vi odd;
    for (int i = 1; i < n; i += 2) odd.pb(row[i]);
    vi oddans = min_smawk_rec(f, odd, col), ans(n);
    F0R (i, size(odd)) ans[2 * i + 1] = oddans[i];
    for (int i = 0, j = 0; i < n; i += 2) {
        ans[i] = col[j];
        int last = i + 1 < n ? ans[i + 1] : col.back();
        while (col[j] != last) {
            j++;
            if (f(row[i], col[j]) < f(row[i], ans[i])) ans[i] = col[j];
        }
    } // <hash>
    return ans;
}
template<class Fn>
vi min_smawk(Fn f, int r, int c) {
    vi row(r), col(c);
    iota(all(row), 0), iota(all(col), 0);
    return min_smawk_rec(f, row, col);
} // <hash>

// Compute min plus convolution c[k] = min{i+j=k}(a[i]+b[j]) for convex b. O(N + M)
template<class V>
vt<V> min_plus_smawk(const vt<V>& a, const vt<V>& b) {
    int n = size(a), m = size(b);
    if (!n || !m) return n ? a : b;
    auto f = [&] (int r, int c) -> tuple<int, V, int> {
        if (r < c) return {1, V{}, c};
        if (r - c >= m) return {1, V{}, -c};
        return {0, a[c] + b[r - c], 0};
    };
    vi cols = min_smawk(f, n + m - 1, n);
    vt<V> d(n + m - 1);
    F0R (r, n + m - 1) d[r] = a[cols[r]] + b[r - cols[r]];
    return d;
} // <hash>

// Compute min plus convolution c[k] = min{i+j=k}(a[i]+b[j]) for concave b. O(N log M + M)
template<class V>
vt<V> min_plus_concave_one(const vt<V>& a, const vt<V>& b) {
    int n = size(a), m = size(b), z = n + m - 1;
    if (!n || !m) return n ? a : b;
    vt<V> c(z, INF); // V = ll
    auto solve = [&] (int l, int r, bool rev) {
        auto val = [&] (int j, int k) {
            if (rev) j = n - 1 - j, k = z - 1 - k;
            return a[j] + b[k - j];
        }; // <hash>
#define better(i,j,k) val(i,k) <= val(j,k)
        auto improve = [&] (int u, int v, int l, int r) {
            while (r - l > 1) {
                int m = (l + r) / 2;
                (better(u, v, m) ? r : l) = m;
            }
            return l;
        };
        vt<array<int, 2>> stk;
        for (int i = l, k = l; k < r; i++, k++) {
            while (size(stk) && i < n && better(i, stk.back()[0], stk.back()[1]))
                stk.pop_back();
            if (i < n) {
                int t = stk.empty() ? r - 1 : improve(stk.back()[0], i, k - 1, stk.back()[1]);
                if (t >= k) stk.pb({i, t});
            } // <hash>
            int out = rev ? z - 1 - k : k;
            c[out] = min(c[out], val(stk.back()[0], k));
            if (stk.back()[1] == k) stk.pop_back();
        }
    };

    solve(0, m, 0), solve(0, m, 1);
    for (int k = m; k < z - m; k += m + 1)
        solve(k + 1, k + m + 1, 0), solve(z - k - m, z - k, 1);
#undef better
    return c;
}
