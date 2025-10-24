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

#ifdef __cplusplus
extern "C"
#endif
const char* __asan_default_options() { return "detect_leaks=0"; }

#include "../../../content/data-structures/Trie.h"

signed main() {
    cin.tie(0)->sync_with_stdio(0);
    
    int q; cin >> q;
    ptr trie = new Node {};
    while (q--) {
        int t, x; cin >> t >> x;
        if (t == 0) ins(trie, x);
        if (t == 1) del(trie, x);
        if (t == 2) cout << qmin(trie, x) << '\n';
        if (t == 3) cout << qmax(trie, x) << '\n';
        if (t == 4) {
            int k; cin >> k;
            cout << count<0>(trie, x, k) << '\n';
        }
        if (t == 5) {
            int k; cin >> k;
            cout << count<1>(trie, x, k) << '\n';
        }
    }
}