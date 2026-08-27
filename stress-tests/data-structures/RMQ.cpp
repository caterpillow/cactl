// Tests SparseTable.h (RMQ): all-pairs min queries vs brute for n=0..100
// (n=1 included; n=0 is rejected by an assert). written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/data-structures/SparseTable.h"

int main() {
    srand(2);
    FOR (N, 1, 101) { // N == 0 asserts by design
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
