// Tests FunnySparseTable.h: all-pairs min queries (incl. width 1) vs brute,
// n=1..100. written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/data-structures/FunnySparseTable.h"

int main() {
    srand(3);
    FOR (N, 1, 101) {
        vi v(N);
        F0R (i, N) v[i] = i;
        random_shuffle(all(v));
        RMQ<int> rmq;
        rmq.init(v);
        F0R (i, N) FOR (j, i + 1, N + 1) {
            int n = inf;
            FOR (k, i, j) n = min(n, v[k]);
            assert(n == rmq.query(i, j));
        }
    }
    cout << "Tests passed!" << endl;
}
