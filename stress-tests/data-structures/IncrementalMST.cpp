// Tests IncrementalMST.h two ways:
// (A) as offline dynacon per its description: weights = (negated) distinct
//     deletion times, max spanning forest by deletion time; random toggle/query
//     timelines vs brute DFS component count.
// (B) merge()-only incremental MST: forest weight after each insertion vs
//     Kruskal on the prefix (distinct weights), plus connectivity vs brute.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/data-structures/IncrementalMST.h"

int brute_comps(int n, vt<pi>& eds) {
    vi p(n);
    iota(all(p), 0);
    function<int(int)> find = [&](int x) { return p[x] == x ? x : p[x] = find(p[x]); };
    int c = n;
    for (auto [u, v] : eds) if (find(u) != find(v)) p[find(u)] = find(v), c--;
    return c;
}

int comps(DSU& d, int n) {
    set<int> r;
    F0R(i, n) r.insert(d.find(i));
    return size(r);
}

int main() {
    mt19937 rng(999);
    // (A) offline dynacon
    F0R(cs, 5000) {
        int n = rng() % 7 + 2, T = 30;
        // build timeline: op = (type, u, v); type 0 toggle, 1 query
        vt<array<int, 3>> ops;
        set<pi> act;
        F0R(i, T) {
            if (rng() % 3 == 0) ops.pb({1, 0, 0});
            else {
                int u = rng() % n, v = rng() % n;
                if (u == v) { ops.pb({1, 0, 0}); continue; }
                if (u > v) swap(u, v);
                ops.pb({0, u, v});
                if (act.count({u, v})) act.erase({u, v}); else act.insert({u, v});
            }
        }
        // deletion time per insertion op (distinct); never-deleted -> T + k
        vi del(T, -1);
        map<pi, int> ins; // pair -> insertion op index
        int extra = T;
        F0R(i, T) {
            auto [t, u, v] = ops[i];
            if (t) continue;
            if (ins.count({u, v})) { del[ins[{u, v}]] = i; ins.erase({u, v}); }
            else ins[{u, v}] = i;
        }
        for (auto [e, i] : ins) del[i] = extra++;
        // run
        DSU dsu(n);
        map<pi, int> curw;   // active pair -> its weight (-deletion time)
        set<int> instruct;   // weights currently held by the structure
        vt<pi> alive;
        F0R(i, T) {
            auto [t, u, v] = ops[i];
            if (t) { assert(comps(dsu, n) == brute_comps(n, alive)); continue; }
            pi e{u, v};
            if (curw.count(e)) { // deletion op
                int w = curw[e];
                curw.erase(e);
                alive.erase(find(all(alive), e));
                if (instruct.count(w)) dsu.delete_max_edge(u, v, w), instruct.erase(w);
            } else {
                int w = -del[i];
                curw[e] = w;
                alive.pb(e);
                pi r = dsu.merge(u, v, {w, i});
                if (r.f == inf) instruct.insert(w);           // added, no eviction
                else if (r.f != w) instruct.erase(r.f), instruct.insert(w); // swap
                // r.f == w: rejected (its cycle-mates all outlive it) -> not in structure
            }
        }
    }
    // (B) merge-only incremental MST vs Kruskal
    F0R(cs, 5000) {
        int n = rng() % 8 + 2, m = 15;
        DSU dsu(n);
        vt<int> ws(m);
        iota(all(ws), 1);
        shuffle(all(ws), rng);
        ll tot = 0;
        vt<array<int, 3>> eds; // (w, u, v) prefix
        F0R(i, m) {
            int u = rng() % n, v = rng() % n, w = ws[i];
            eds.pb({w, u, v});
            pi r = dsu.merge(u, v, {w, i});
            if (r.f == inf) tot += w;
            else if (r.f != w || r.s != i) tot += w - r.f;
            // Kruskal on prefix
            sort(all(eds));
            vi p(n);
            iota(all(p), 0);
            function<int(int)> find = [&](int x) { return p[x] == x ? x : p[x] = find(p[x]); };
            ll bt = 0;
            for (auto [w2, a, b] : eds)
                if (find(a) != find(b)) p[find(a)] = find(b), bt += w2;
            assert(tot == bt);
            vt<pi> pe;
            for (auto [w2, a, b] : eds) pe.pb({a, b});
            assert(comps(dsu, n) == brute_comps(n, pe));
        }
    }
    cout << "Tests passed!" << endl;
}
