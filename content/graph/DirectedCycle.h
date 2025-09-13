/**
 * Author: Benq
 * Description: stack
 * Source: https://www.geeksforgeeks.org/detect-cycle-in-a-graph/
 */

template<int sz> struct DirCyc {
	vi adj[sz], stk, cyc; vt<bool> in_stk, vis; 
	void dfs(int x) {
		stk.pb(x); in_stk[x] = vis[x] = 1;
        for (int i : adj[x]) {
			if (in_stk[i]) cyc = {find(all(stk),i), end(stk)};
			else if (!vis[i]) dfs(i); 
			if (size(cyc)) return;
		}
		stk.pop_back(); in_stk[x] = 0;
	}
	vi init(int n) {
		in_stk.resize(n), vis.resize(n); 
		F0R (i, n) if (!vis[i] && !size(cyc)) dfs(i);
		return cyc;
	}
};