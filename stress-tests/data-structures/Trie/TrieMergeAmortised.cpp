// Checks that Trie.h merge() work is bounded by nodes allocated:
// every merge call with both sides non-null retires one node of r, so
// #calls <= 3*allocs + #top-level merges. Nodes are counted by traversal
// before merging (the trie allocates only via c() and never frees).
// Written by Claude (audit).
#include "../../utilities/template.h"
#undef sz
#include "../../../content/data-structures/Trie.h"
ll calls = 0;
int merge_counted(ptr &l, ptr r, int i = lg) { // header's merge + counter
	calls++;
	if (!l) swap(l, r);
	if (!l) return 0;
	if (!r) return l->cnt;
	if (!i) return l->cnt += r->cnt;
	l->push(i), r->push(i);
	l->cnt = 0;
	F0R (j, 2) l->cnt += merge_counted(l->_c[j], r->_c[j], i - 1);
	return l->cnt;
}
ll nodes(ptr t) { return t ? 1 + nodes(t->_c[0]) + nodes(t->_c[1]) : 0; }
mt19937 rng(7);
int main() {
	F0R (shape, 3) {
		int K = 300, M = 400; ll allocs = 0; calls = 0;
		vt<ptr> ts; vt<multiset<int>> bs(K);
		F0R (k, K) {
			ptr t = new Node{};
			F0R (j, M) {
				int x = shape == 0 ? rng() % (1 << 30)              // random
					: shape == 1 ? rng() % 64                        // heavy sharing
					: (int) (rng() % 2 ? j : (1 << 29) + j);         // deep shared paths
				t->multi_ins(x); bs[k].insert(x);
			}
			if (rng() % 3 == 0) {
				int v = rng() % (1 << 30); t->lazy ^= v;
				multiset<int> nb; for (int y : bs[k]) nb.insert(y ^ v); bs[k] = nb;
			}
			allocs += nodes(t); ts.pb(t);
		}
		ptr acc = ts[0];
		FOR (k, 1, K) merge_counted(acc, ts[k]), bs[0].insert(all(bs[k]));
		printf("shape %d: allocs %lld, merge calls %lld, ratio %.2f\n",
			shape, allocs, calls, (double) calls / allocs);
		assert(calls <= 3 * allocs + K);
		assert(acc->cnt == size(bs[0]));
		F0R (q, 2000) { // merged trie answers agree with brute force
			int x = rng() % (1 << 30), best = INT_MAX;
			for (int y : bs[0]) best = min(best, x ^ y);
			assert(acc->qmin(x) == best);
		}
	}
	// merging small tries into a big one must not re-traverse the big one
	ptr big = new Node{}; F0R (j, 20000) big->ins(rng() % (1 << 30));
	ll small = 0; calls = 0;
	F0R (r, 50) {
		ptr t = new Node{}; F0R (j, 5) t->ins(rng() % (1 << 30));
		small += nodes(t); merge_counted(big, t);
	}
	printf("incremental: small-trie nodes %lld, calls %lld\n", small, calls);
	assert(calls <= 3 * small + 50);
	puts("Tests passed!");
}
