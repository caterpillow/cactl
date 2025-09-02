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
 *  ts.either(0, \tilde3); // Var 0 is true or var 3 is false
 *  ts.setValue(2); // Var 2 is true
 *  ts.atMostOne({0,\tilde1,2}); // <= 1 of vars 0, \tilde1 and 2 are true
 *  ts.solve(); // Returns true iff it is solvable
 *  ts.values[0..N-1] holds the assigned values to the vars
 * Time: O(N+E), where N is the number of boolean variables, and E is the number of clauses.
 * Status: stress-tested
 */
#pragma once

struct TwoSAT {
    int n = 0; 
    vt<pi> edges;
    void init(int _n) { n = _n; }
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
    void at_most_one(const vt<int>& li) { 
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
        SCC scc; 
        scc.init(2 * n);
        for(auto& e : edges) {
            scc.ae(e.f ^ 1, e.s);
            scc.ae(e.s ^ 1, e.f);
        }
        scc.gen(); 
        reverse(all(scc.comps)); // reverse topo order
        for (int i = 0; i < 2 * n; i += 2) 
            if (scc.comp[i] == scc.comp[i ^ 1]) return {};
        vt<int> tmp(2 * n); 
        for (auto i : scc.comps) {
            if (!tmp[i]) tmp[i] = 1, tmp[scc.comp[i ^ 1]] = -1;
        }
        vt<bool> ans(n); 
        F0R (i, n) ans[i] = tmp[scc.comp[2 * i]] == 1;
        return ans;
    }
};