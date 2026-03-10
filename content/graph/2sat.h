/**
 * Author: Emil Lenngren, Simon Lindholm
 * Date: 2011-11-29
 * License: CC0
 * Source: folklore
 * Description: Calculates a valid assignment to boolean variables a, b, c,... to a 2-SAT problem,
 * so that an expression of the type $(a||b)\&\&(!a||c)\&\&(d||!b)\&\&...$
 * becomes true, or reports that it is unsatisfiable.
 * Negated variables are represented by bit-inversions (\texttt{\tilde{}x}).
 * Usage:
 *  TwoSat ts(number of boolean variables);
 *  ts.either(0, \tilde3); // var 0 is true or var 3 is false
 *  ts.force(2); // var 2 is true
 *  ts.at_most_one({0,\tilde1,2}); // <= 1 of vars 0, \tilde1 and 2 are true
 *  ts.solve(); // returns true iff it is solvable
 *  ts.values[0..N-1] holds the assigned values to the vars
 * Time: O(N+E), where N is the number of boolean variables, and E is the number of clauses.
 * Status: stress-tested
 */
#pragma once

// using G = vt<basic_string<int>>;
using pi = pair<int, int>;
struct TwoSAT {
    int n; 
    vt<pi> edges;
    // TwoSat sat{n};
    int add() { return n++; }
    void either(int x, int y) { // x | y
        x = max(2 * x, -1 - 2 * x); // ~(2 * x)
        y = max(2 * y, -1 - 2 * y); // ~(2 * y)
        edges.pb({x, y}); 
    }
    void implies(int x, int y) { either(~x, y); } 
    void force(int x) { either(x, x); } 
    void exactly_one(int x, int y) { 
        either(x, y), either(~x, ~y); 
    }
    void tie(int x, int y) { 
        implies(x, y), implies(~x, ~y); 
    } 
    void nand(int x, int y ) { either(~x, ~y); } 
    void at_most_one(const vi &li) { 
        if (size(li) <= 1) return;
        int cur = ~li[0];
        FOR (i, 2, size(li)) {
            int next = add();
            either(cur, ~li[i]); 
            either(cur,next);
            either(~li[i], next); 
            cur = ~next;
        }
        either(cur, ~li[1]);
    }
    vt<bool> solve() {
        G adj(2 * n);
        for(auto& e : edges) {
            adj[e.f ^ 1].pb(e.s);
            adj[e.s ^ 1].pb(e.f);
        }
        SCC scc(2 * n, adj);
        reverse(all(scc.comps)); // reverse topo order
        for (int i = 0; i < 2 * n; i += 2) 
            if (scc.comp[i] == scc.comp[i ^ 1]) return {};
        vi tmp(2 * n); 
        for (auto i : scc.comps) {
            if (!tmp[i]) tmp[i] = 1, tmp[scc.comp[i ^ 1]] = -1;
        }
        vt<bool> ans(n); 
        F0R (i, n) ans[i] = tmp[scc.comp[2 * i]] == 1;
        return ans;
    }
};