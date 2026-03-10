#include <bits/stdc++.h>
 
using namespace std;
 
using ll = long long;
using pl = pair<ll, ll>;
#define vt vector
#define f first
#define s second
#define all(x) x.begin(), x.end() 
#define pb push_back
#define vt vector
#define FOR(i, a, b) for (int i = (a); i < (b); i++)
#define ROF(i, a, b) for (int i = (b) - 1; i >= (a); i--)
#define F0R(i, b) FOR (i, 0, b)
#define endl '\n'
#define debug(x) do{auto _x = x; cerr << #x << " = " << _x << endl;} while(0)
const ll INF = 1e18;
using vi = vt<int>;
using pi = pair<int, int>;
 
struct BCC { 
    int t; 
    vt<vt<pi>> adj;
    vt<vi> comps; // lists of edges of bcc
    vi tin, stk, is_art, is_bridge; // stk for bcc only
 
    void init(int n, vt<pi> &edges) {
        int m = size(edges);
        adj.resize(n);
        F0R (i, m) {
            auto [u, v] = edges[i];
            adj[u].pb({v, i});
            adj[v].pb({u, i});
        }
        t = 0;
        tin = is_art = vi(n);
        is_bridge.resize(m);
        F0R (u, n) if (!tin[u]) dfs(u, -1); 
        // if we include bridges as 2-node bcc
        // F0R (i, m) if (is_bridge[i]) comps.pb({i});
    }
 
    int dfs(int u, int par) {
        int me = tin[u] = ++t, dp = me;
        int cnt = (par != -1); // art
        for (auto [v, ei] : adj[u]) if (ei != par) {
            if (tin[v]) {
                dp = min(dp, tin[v]);
                if (tin[v] < me) stk.pb(ei); // bcc
            } else {
                int si = size(stk), up = dfs(v, ei);
                dp = min(dp, up);
                cnt += up >= me; // art
                if (up == me) { // bcc 
                    stk.pb(ei);
                    comps.pb({si + all(stk)});
                    stk.resize(si);
                } else if (up < me) stk.push_back(ei); // bcc
                if (up > me) is_bridge[ei] = 1; // up > me
            }
        }
        is_art[u] = cnt >= 2; // art
        return dp;
    }
};
 
int main() {
    freopen("network.in", "r", stdin);
    freopen("network.out", "w", stdout);
    cin.tie(0)->sync_with_stdio(0);
 
    int n; cin >> n;
    while (n != 0) {
        vt<pair<int, int>> edges;
        string st;
        getline(cin, st);
        while (st != "0") {
            istringstream sin(st);
            int u; sin >> u;
            u--;
            for (int v; sin >> v; ) {
                v--;
                edges.push_back({u, v});
            }
            getline(cin, st);
        }
        BCC bcc; bcc.init(n, edges);
        cout << accumulate(all(bcc.is_art), 0) << '\n';

        cin >> n;
    }
}
