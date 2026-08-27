/**
 * Author: Unknown
 * Date: 2017-05-15
 * Source: https://e-maxx.ru/algo/ukkonen
 * Description: Ukkonen's algorithm for the compact trie of all suffixes.
 *  Edges are intervals $[l,r)$ of the input, so the tree has $O(N)$ nodes.
 *  Each internal node's suffix link removes the first character from its path.
 *  The root is 0; node 1 is auxiliary and not part of the tree.
 *  Append a unique dummy symbol to make every suffix an explicit leaf;
 *  without one, some suffixes end inside edges (substring matching still works).
 *  Characters are consecutive from `a'; pass the alphabet size to the constructor.
 * Time: $O(\Sigma N)$
 * Status: stress-tested
 */
#pragma once

struct SuffixTree {
    int toi(char c) { return c - 'a'; }
    string a; // v = current node, q = current position
    int A, v = 0, q = 0, m = 2;
    vt<vi> t;
    vi l, r, p, suf;

    void ukkadd(int i, int c) { suff:
        if (r[v] <= q) {
            if (t[v][c] == -1) { t[v][c] = m; l[m] = i;
                p[m++] = v; v = suf[v]; q = r[v]; goto suff; }
            v = t[v][c]; q = l[v];
        }
        if (q == -1 || c == toi(a[q])) q++; else {
            l[m + 1] = i; p[m + 1] = m; l[m] = l[v]; r[m] = q;
            p[m] = p[v]; t[m][c] = m + 1; t[m][toi(a[q])] = v;
            l[v] = q; p[v] = m; t[p[m]][toi(a[l[m]])] = m;
            v = suf[p[m]]; q = l[m];
            while (q < r[m]) { v = t[v][toi(a[q])]; q += r[v] - l[v]; }
            suf[m] = q == r[m] ? v : m + 2;
            q = r[v] - (q - r[m]); m += 2; goto suff;
        }
    }

    SuffixTree(string a, int alpha = 27) : a(a), A(alpha),
        t(max(2, 2 * size(a) + 1), vi(A, -1)), l(size(t)), r(size(t), size(a)),
        p(size(t)), suf(size(t)) {
        F0R (c, A) t[1][c] = 0;
        suf[0] = 1; l[0] = l[1] = -1;
        r[0] = r[1] = p[0] = p[1] = 0;
        F0R (i, size(a)) ukkadd(i, toi(a[i]));
    }

	// Example: longest common substring as {length, start in s}; assumes a-z.
    pi best;
    int lcs(int node, int i1, int i2, int olen) {
        if (l[node] <= i1 && i1 < r[node]) return 1;
        if (l[node] <= i2 && i2 < r[node]) return 2;
        int mask = 0, len = node ? olen + r[node] - l[node] : 0;
        F0R (c, A) if (t[node][c] != -1)
            mask |= lcs(t[node][c], i1, i2, len);
        if (mask == 3) best = max(best, pi{len, r[node] - len});
        return mask;
    }
    static pi LCS(string s, string t) {
        SuffixTree st(s + char('z'+1) + t + char('z'+2), 28);
        st.lcs(0, size(s), size(s) + 1 + size(t), 0);
        return st.best;
    }
};
