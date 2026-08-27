/** 
 * Author: caterpillow
 * Description: Standalone program (needs the template): reads $n$ and $n-1$ edges
 * (1-indexed), prints for every node its maximum distance to another node.
 */

#include <bits/stdc++.h>

using namespace std;

int main() {
    cin.tie(0)->sync_with_stdio(0);

    int n;
    cin >> n;
    vt<vi> adj(n);
    for (int i = n - 1; i--;) {
        int u, v;
        cin >> u >> v;
        adj[--u].pb(--v);
        adj[v].pb(u);
    }
    vi dp1(n, 0), dp2(n);
    auto dfs1 = [&] (auto &&self, int u, int p) -> void {
        for (int v : adj[u]) if (v != p) {
            self(self, v, u);
            dp1[u] = max(dp1[u], dp1[v] + 1);
        }
    };
    auto dfs2 = [&] (auto &&self, int u, int p) -> void {
        multiset<int> dps {dp2[u]};
        for (int v : adj[u]) if (v != p) dps.insert(dp1[v] + 1);
        for (int v : adj[u]) if (v != p) {
            if (*dps.rbegin() == dp1[v] + 1) dp2[v] = *next(dps.rbegin()) + 1;
            else dp2[v] = *dps.rbegin() + 1;
            self(self, v, u);
        }
    };
    dfs1(dfs1, 0, -1);
    dfs2(dfs2, 0, -1);
    for (int i = 0; i < n; i++) {
        cout << max(dp1[i], dp2[i]) << " \n"[i == n - 1];
    }
}