// Stress-test harness template (rewritten during the 2026-08 audit, by Claude).
// Provides the contest template's macros plus legacy KACTL shims so both old
// and new headers compile in tests.
#include <bits/stdc++.h>
using namespace std;
using ll = long long;
using db = double;

#define FOR(i, a, b) for (int i = a; i < (b); i++)
#define F0R(i, b) FOR(i, 0, b)
#define all(x) begin(x), end(x)
#define vt vector
#define size(x) ((int) (x).size())
#define ROF(i, a, b) for (int i = (b) - 1; i >= (a); i--)
#define pb push_back
#define f first
#define s second
using vi = vt<int>;
using vl = vt<ll>;
using pi = pair<int, int>;
using pl = pair<ll, ll>;
const int inf = 1e9;
const ll INF = 1e18;

// legacy KACTL shims
#define rep(i, from, to) FOR(i, from, to)
#define sz(x) size(x)
typedef pair<int, int> pii;
