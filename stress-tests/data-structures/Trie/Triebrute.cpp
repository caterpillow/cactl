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

signed main() {
    cin.tie(0)->sync_with_stdio(0);
    
    int q; cin >> q;
    set<int> a;
    while (q--) {
        int t, x; cin >> t >> x;
        if (t == 0) a.insert(x);
        if (t == 1) a.erase(x);
        if (t == 2) {
            int ans = INT_MAX;
            for (int y : a) ans = min(ans, y ^ x);
            cout << ans << '\n';
        }
        if (t == 3) {
            int ans = -1;
            for (int y : a) ans = max(ans, y ^ x);
            cout << ans << '\n';
        }
        if (t == 4) {
            int k; cin >> k;
            int ans = 0;
            for (int y : a) ans += (x ^ y) < k;
            cout << ans << '\n';
        }
        if (t == 5) {
            int k; cin >> k;
            int ans = 0;
            for (int y : a) ans += (x ^ y) > k;
            cout << ans << '\n';
        }
    }
}