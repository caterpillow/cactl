# Rope.h: what was measured

Harness: `doc/scripts/ropebench.cpp` (compile line in its header). AMD Ryzen 9
9950X3D, g++ 13.3.0, `-O2 -std=c++17`, 2026-09-13. Every scenario runs in its own
child process 3 times, median reported; MB is the child's peak RSS (`ru_maxrss`,
baseline 1.5 MB), so it is the peak of one run including the input arrays. Positions
come from a fixed-seed mt19937 and the same op stream is checksummed on all
structures. Baselines: `std::vector` (memmove), an implicit treap with split/merge
(random priorities, kth by size), and for H a persistent segment tree (point
update/query, 12 bytes per node). Times in ms. `-O3 -march=native` was run once:
rope and treap are within noise, only vector's memmove/rotate gets 2-5x faster
(noted where it matters). Whole run: 6 min 50 s; no scenario was skipped.

## A. build

| | rope `push_back` | `rope(ptr, N)` | treap | vector |
|---|---:|---:|---:|---:|
| N = 1e5 | 1.2 / 3.2 MB | 0.0 / 2.2 MB | 6.3 / 4.5 MB | 0.2 / 2.1 MB |
| N = 1e6 | 12.8 / 18.5 MB | 0.2 / 9.2 MB | 69 / 25.5 MB | 1.0 / 9.3 MB |

The ctor copies the array into a single leaf (4 MB for 1e6 ints); `push_back`
makes leaves of at most 23 elements, 9 MB of nodes on top.

## B. Q random `insert(pos, val)` from empty, C. Q random `erase(pos, 1)` from size Q

| Q | B rope | B treap | B vector | C rope | C treap | C vector |
|---|---:|---:|---:|---:|---:|---:|
| 1e5 | 136 | 30 | 49 | 85 | 29 | 51 |
| 3e5 | 521 | 126 | 454 | 309 | 109 | 469 |
| 1e6 | 2607 | 556 | - | 2003 | 483 | - |

Per op: rope 1.4-2.6 us insert, 0.85-2.0 us erase; treap 0.3-0.55 us. Memory is
the same for all three tree-ish rows (25-31 MB at 1e6).

## D. mixed, N = 1e5 steady state, Q = 1e6 (50/50 insert/erase)

| rope | treap | vector |
|---:|---:|---:|
| 1397 / 5.6 MB | 434 / 22 MB | 990 / 2.3 MB |

## E. cut-and-paste, Q = 1e5 x (`substr(l, len)` + `erase(l, len)` + `insert(pos, block)`)

Treap: three splits and three merges; vector: one `std::rotate` (the contestant's
version, touching only the affected range, not erase+insert).

| N | len in | paste at | rope | treap | vector rotate (-O3) |
|---|---|---|---:|---:|---:|
| 1e5 | [1, 100] | random | 297 | 89 | 734 (412) |
| 1e5 | [1, 100] | 0 | 207 | 57 | 1132 (626) |
| 1e5 | [1, N/10] | random | 277 | 105 | 758 (158) |
| 1e5 | [1, N/10] | 0 | 195 | 73 | 1122 (220) |
| 1e6 | [1, 100] | random | 643 | 142 | 7942 (4183) |
| 1e6 | [1, 100] | 0 | 313 | 88 | 11699 (6361) |
| 1e6 | [1, N/10] | random | 805 | 189 | 8201 (1739) |
| 1e6 | [1, N/10] | 0 | 521 | 136 | 11456 (2518) |

The block length does not matter for the rope (substr shares subtrees); the
distance moved does not matter either. Peak memory 4-6 MB at 1e5, 25-33 MB at 1e6
for rope and treap, 2 / 5 MB for vector.

## F. 1e6 random reads `r[i]`

"fragmented" = the push_back rope after 1e6 random inserts and 1e6 random erases
(same size, ~1e5 or ~1e6 small leaves).

| N | rope built | rope fragmented | treap kth | vector |
|---|---:|---:|---:|---:|
| 1e5 | 75 / 7 MB | 126 / 37 MB | 102 / 8.5 MB | 0.2 |
| 1e6 | 159 / 22 MB | 313 / 58 MB | 209 / 25 MB | 0.4 |

Rope reads are 0.08-0.16 us on a rope built in order (faster than the treap: the
leaves are 23-element arrays, so the tree is shallower), 0.13-0.31 us fragmented.
Note the 37 MB for a fragmented rope of 1e5 ints: 1-element leaves are ~60 bytes
plus a ~48-byte concat node each, and the allocator keeps the freed memory.

## G. sequential iteration, N = 1e6

| rope built, range-for | `rope(ptr, N)`, range-for | rope fragmented, range-for | treap inorder | vector |
|---:|---:|---:|---:|---:|
| 2.0 | 0.4 | 11.6 | 5.6 | 0.2 |

Loop over `r[i]` for i = 0..N-1 at N = 1e5: 1.1 ms built, 1.3 ms fragmented, i.e.
11-13 ns per element -- the "anti-pattern" is 5-10x slower than the range-for (0.2
ms for 1e5) but not a disaster at 1e5: sequential `r[i]` walks a warm tree.

## H. persistent array: V versions, each a copy of a version + one `replace(x, v)`, then 1e6 reads `ver[j][x]`

"random parent" copies a uniformly random earlier version, "chain" copies the
previous one (a normal persistent-array workload, V edits in sequence).

| N | V | parent | rope versions | rope reads | rope MB | segtree versions | segtree reads | segtree MB |
|---|---|---|---:|---:|---:|---:|---:|---:|
| 1e5 | 1e5 | random | 82 | 199 | 171 | 12 | 55 | 37 |
| 1e5 | 1e5 | chain | 128 | 1881 | 434 | 13 | 65 | 37 |
| 1e5 | 1e6 | random | 1208 | 349 | 1700 | 281 | 137 | 233 |
| 1e5 | 1e6 | chain | 1495 | 2588 | 4269 | 248 | 540 | 233 |
| 1e6 | 1e5 | random | 98 | 220 | 180 | 41 | 227 | 61 |
| 1e6 | 1e5 | chain | 225 | 2371 | 530 | 29 | 224 | 61 |
| 1e6 | 1e6 | random | 1349 | 367 | 1715 | 563 | 265 | 293 |
| 1e6 | 1e6 | chain | 3283 | 3280 | 5257 | 399 | 563 | 293 |

Per version the rope costs 1.7 KB (random parent) to 4.3-5.3 KB (chain), the
segment tree 0.23 KB (one path of 20 nodes); a `replace` is
`substr(0, p) + leaf + substr(p+1, n)`, i.e. two path copies of ~48-byte concat
nodes plus the concat nodes joining them. In a chain each version is one level
deeper than its parent, so the tree sits at the 45-level cap and is rebalanced every
~25 versions: reads on chain versions are 1.9-3.3 us instead of 0.2-0.4, and each
rebalance copies the whole spine, which is where the 4-5 KB per version go.

## I. element types, Q = 3e5 random inserts / 1e6 random reads

| type | rope insert | treap insert | vector insert | rope reads, built N = 1e6 | rope reads after the 3e5 inserts |
|---|---:|---:|---:|---:|---:|
| int (from B/F) | 521 | 126 | 454 | 159 | - |
| char | 534 / 8.2 MB | 128 | 111 | 155 | 141 |
| ll | 572 / 10.7 MB | 128 | 1027 | 164 | 147 |
| struct {int, int} | 533 / 10.7 MB | 122 | 1029 | 185 | 147 |

The element type is irrelevant for the rope (node overhead dominates). `Rope<char>`
gets no shortcut; a `vector<char>` is 5x faster than the rope at 3e5 inserts.
`Rope<pair<int,int>>` **does not compile** on g++ 13 (see below); the row uses a
plain struct.

## J. degradation: N = 1e6, 10 rounds of (1e5 random inserts, 1e5 random reads)

| round | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | 10 |
|---|---:|---:|---:|---:|---:|---:|---:|---:|---:|---:|
| push_back rope, inserts | 324 | 358 | 366 | 365 | 377 | 436 | 434 | 423 | 408 | 417 |
| push_back rope, reads | 35 | 37 | 39 | 37 | 37 | 51 | 54 | 45 | 49 | 58 |
| ctor rope, inserts | 313 | 373 | 360 | 397 | 408 | 399 | 405 | 405 | 407 | 432 |
| ctor rope, reads | 36 | 35 | 38 | 43 | 43 | 42 | 48 | 47 | 47 | 57 |

Size goes 1e6 -> 2e6 (log factor +5%); inserts get 30-40% slower, reads 60%. That
is fragmentation (more, smaller leaves) rather than depth blowup: the depth cap and
the rebalance keep it from going superlinear; round 10 is 4.2 us per insert at
size 2e6, against 2.6 us averaged over scenario B's growth from 0 to 1e6. No cliff in 10 rounds; a second run of 10 rounds
was not tried.

## What this means in a contest

- The rope is a treap you do not have to type: 3-5x slower than an implicit treap
  on every update (1.4-3 us per insert/erase/cut-paste vs 0.3-0.9), similar or
  faster on random reads, and 0.5-0.8 s per 3e5 updates -- fine for Q <= 3e5 in a
  2 s limit, marginal at Q = 1e6 (2-3 s for inserts alone).
- `vector` with memmove beats the rope on single inserts/erases up to n ~ 2e5
  (rope wins from 3e5) and is competitive on block moves at n = 1e5 if you use
  `std::rotate`; at n = 1e6 the rope is 10-30x faster than any vector solution.
- Iterate with a range-for (2 ms per 1e6, 12 ms fragmented). `r[i]` in a loop costs
  0.1-0.3 us each, 100x the vector but not catastrophic; avoid it in the inner loop.
- Build with `Rope<int>(v.data(), size(v))`, not N `push_back`s: instant, half the
  memory, 5x faster to iterate.
- Persistent array: 1.2 us and 1.7 KB per version if versions branch randomly, 1.3-3.3
  us and 4-5 KB per version for a chain of edits, reads 0.2-0.4 us (random) or 2-3 us
  (chain). A persistent segment tree is 4-8x faster and uses 7-20x less memory
  (0.23 KB/version). In a 256 MB limit the rope holds ~1e5 versions of a random
  tree, ~5e4 of a chain; 1e6 versions need 1.7-5.3 GB. Use it for V <= 1e5, write
  the segment tree above that.
- Memory: 25-30 MB per 1e6 elements after random inserts, 58 MB fragmented (~60
  bytes per 1-element leaf); a `rope<char>` is not smaller than a `rope<int>`.

## Dodgy

- `Rope<pair<int,int>>` (and any element type from namespace `std`: tuple, array,
  string) fails to compile on g++ 13.3 in every `-std=` mode: `uninitialized_copy_n`
  inside `ropeimpl.h` is ambiguous between `std::` (found by ADL on the element
  type) and `__gnu_cxx::`'s own. A user struct `struct P { int a, b; }` compiles.
- Persistent chains: a rope that is only ever `copy + replace`d sits at the
  45-level depth cap, gets rebalanced every ~25 versions and each rebalance copies
  ~1e5 concat nodes into memory that old versions keep alive: 5.3 GB for 1e6
  versions at N = 1e6. Reads on those versions are 10x slower than on random
  branches. No crashes, no nonlinear time (build stays ~linear in V).
- Fragmented ropes hold ~60 bytes per element and the allocator does not return the
  memory freed by erases (37 MB peak for a 1e5-element rope that went through 1e6
  inserts + erases).
- J shows a mild creep (30-60% over 10 rounds of 1e5 inserts on 1e6 elements)
  that is fragmentation, not superlinear growth.
- All erase calls in the harness are `erase(pos, n)`; the one-argument `erase(i)`
  is not measured because it does not exist on this compiler (see Rope.h).
