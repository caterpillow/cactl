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
 
struct Arts : vi { 
    int t; 
    vi tin;
    Arts(int n, vt<vi> &adj) : vi(n), t(0), tin(n) {
        F0R (u, n) if (!tin[u]) dfs(u, adj, 0); 
    }
 
    int dfs(int u, vt<vi> &adj, int cnt = 1) {
        int dp = tin[u] = ++t;
        for (int v : adj[u]) {
            if (tin[v]) dp = min(dp, tin[v]);
            else {
                int up = dfs(v, adj);
                dp = min(dp, up);
                cnt += up >= tin[u]; 
            }
        }
        at(u) = cnt >= 2;
        return dp;
    }
};
 
int main() {
    freopen("network.in", "r", stdin);
    freopen("network.out", "w", stdout);
    cin.tie(0)->sync_with_stdio(0);
 
    int n; cin >> n;
    while (n != 0) {
        vt<vi> adj(n);
        string st;
        getline(cin, st);
        while (st != "0") {
            istringstream sin(st);
            int u; sin >> u;
            u--;
            for (int v; sin >> v; ) {
                v--;
                adj[u].pb(v);
                adj[v].pb(u);
            }
            getline(cin, st);
        }
        Arts arts(n, adj);
        cout << accumulate(all(arts), 0) << '\n';
        cin >> n;
    }
}
