#include <bits/stdc++.h>

using namespace std;
using ll = long long;

#define vt vector
#define f first
#define s second
#define pb push_back
#define all(x) begin(x), end(x)
#define size(x) ((int) (x).size())
#define FOR(i, a, b) for (int i = (a); i < (b); i++)
#define ROF(i, a, b) for (int i = (b) - 1; i >= (a); i--)
#define F0R(i, b) FOR (i, 0, b)
const int inf = 1e9;
const ll INF = 1e18;
using vi = vt<int>;

vi cs;

using G = vt<basic_string<int>>;
struct SCC {
    int n; 
    G &adj, radj;
    vi todo, comp;
    SCC (G &adj) : adj(adj), n(size(adj)), radj(n), comp(n), todo(n) {
        F0R (i, size(adj)) dfs(i);
        for (int u : todo) rdfs(u, u); 
    }
    void dfs(int u) {
        if (comp[u]--) return;
        for (int v : adj[u]) dfs(v), radj[v].pb(u); 
        todo[--n] = u;
    }
    void rdfs(int u, int w) {
        if (comp[u] >= 0) return;
        comp[u] = w; 
        if (u == w) cs.pb(u);
        for (int v : radj[u]) rdfs(v, w); 
    }
};

struct SCC3 {
	int ti = 0; G &adj;
	vi disc, comp, stk;
    SCC3(int n, G &adj) : adj(adj), disc(n), comp(n, -1) {
        F0R (i, n) if (!disc[i]) dfs(i);
        reverse(all(cs));
    }
	int dfs(int u) {
		int low = disc[u] = ++ti; 
        stk.pb(u);
		for (int v : adj[u]) if (comp[v] == -1)
			low = min(low, disc[v] ?: dfs(v)); 
		if (low == disc[u]) {
            cs.pb(u);
			for (int y = -1; y != u;) 
				comp[y = stk.back()] = u, stk.pop_back();
		}
		return low;
	}
};

struct SCC2 {
    int n, tim = 0, h = 0;
    vi val, comp, stk;
    vt<vi> &adj;
    SCC2 (vt<vi> &adj) : adj(adj), n(size(adj)), val(n), comp(n, -1), stk(n) {
        F0R (i, n) if (comp[i] < 0) dfs(i);
        reverse(all(cs));
    }
    int dfs(int u) {
        int low = val[u] = ++tim, s = h; 
        stk[h++] = u;
        for (int v : adj[u]) if (comp[v] < 0)
            low = min(low, val[v] ?: dfs(v));
        if (low == val[u]) {
            cs.pb(u);
            while (h > s) comp[u = stk[--h]] = stk[s];
        }
        return val[u] = low;
    }
};

signed main() {
    cin.tie(0)->sync_with_stdio(0);

    int n, m; cin >> n >> m;
    G adj(n);
    for (int i = 0; i < m; i++) {
        int u, v; cin >> u >> v;
        adj[u].pb(v);
    }
    // Tarjan
    // vt<vi> comps;
    // SCC scc(adj);
    // reverse(all(comps));
    // cout << size(comps) << '\n';
    // for (auto &v : comps) {
    //     cout << size(v) << ' ';
    //     for (int x : v) cout << x << ' ';
    //     cout << '\n';
    // }

    SCC3 scc(n, adj);
    G comps(n);
    F0R (i, n) comps[scc.comp[i]].pb(i);
    cout << size(cs) << '\n';
    for (int i : cs) {
        cout << size(comps[i]) << ' ';
        for (int x : comps[i]) cout << x << ' ';
        cout << '\n';
    }
}