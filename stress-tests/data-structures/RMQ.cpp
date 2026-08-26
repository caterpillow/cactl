// Tests SparseTable.h (RMQ): all-pairs min queries vs brute for n=0..100
// (n=0 init must not crash; includes n=1). written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/data-structures/SparseTable.h"

int main() {
    srand(2);
    F0R (N, 101) {
        vi v(N);
        F0R (i, N) v[i] = i;
        random_shuffle(all(v));
        RMQ<int> rmq;
        rmq.init(v); // N=0: must not crash
        F0R (i, N) FOR (j, i + 1, N + 1) {
            int n = inf;
            FOR (k, i, j) n = min(n, v[k]);
            assert(n == rmq.query(i, j));
        }
    }
    cout << "Tests passed!" << endl;
}
