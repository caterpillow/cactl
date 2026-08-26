// Tests kdTree.h: nearest-neighbor and k-NN vs brute force on random points
// (with duplicates), many builds; n=0 dies cleanly via the assert (checked in
// a forked child). written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/geometry/kdTree.h"
#include <sys/wait.h>
#include <unistd.h>

int main() {
    // n = 0 must assert, not recurse forever
    if (fork() == 0) {
        fclose(stderr); // silence the assert message
        vt<P> v;
        Node t(v.begin(), v.end(), 0);
        _exit(0); // should not get here
    }
    int st; wait(&st);
    assert(WIFSIGNALED(st) && WTERMSIG(st) == SIGABRT);

    mt19937 rng(99);
    ll d2 = 0; // avoid shadow warnings
    auto dist2 = [] (P a, P b) {
        return 1ll * (a[0] - b[0]) * (a[0] - b[0]) + 1ll * (a[1] - b[1]) * (a[1] - b[1]);
    };
    F0R (build, 20000) {
        // note: |coord| must stay <= ~7e8 so max dist2 < INF sentinel
        int n = 1 + rng() % 40, lim = build % 2 ? 10 : 600000000;
        vt<P> pts(n);
        for (P& p : pts) p = {(int) (rng() % lim) - lim / 2, (int) (rng() % lim) - lim / 2};
        vt<P> tmp = pts;
        Node* tree = new Node(tmp.begin(), tmp.end(), 0);
        F0R (q, 5) {
            P p = {(int) (rng() % lim) - lim / 2, (int) (rng() % lim) - lim / 2};
            vl bd; // brute distances
            for (P& a : pts) bd.pb(dist2(a, p));
            sort(all(bd));
            ll best = INF;
            tree->search(p, best);
            assert(best == bd[0]); // nearest neighbor
            int k = 1 + rng() % 5;
            priority_queue<ll> pq;
            F0R (i, k) pq.push(INF);
            tree->search(p, pq);
            ROF (i, 0, k) { // pq holds the k smallest (INF-padded if k > n)
                d2 = pq.top(); pq.pop();
                assert(d2 == (i < n ? bd[i] : INF));
            }
        }
    }
    cout << "Tests passed!" << endl;
}
