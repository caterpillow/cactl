# Dinic2.h: what was measured

Harness: `doc/scripts/flowbench.cpp` (see its header for the compile line). Seven
graph families, capacities uniform in [1, 1e9] unless stated, min of 5 runs, g++
-O2, 2026-09-10. Times in ms.

| family | Dinic.h | Dinic.h, `int L = 30` (no scaling) | PushRelabel.h | Dinic2.h |
|---|---:|---:|---:|---:|
| random sparse, 1e5 V / 4e5 E | 471 | 46.5 | 190 | **28.7** |
| random dense, 2e3 V / 5e5 E | 10.7 | 2.3 | 12.1 | **1.1** |
| bipartite matching, 8e4 V / 4e5 E, unit | 61.3 | 60.5 | 51.6 | **39.1** |
| grid 400x400, both directions | 13.0 | 6.4 | 317 | **3.3** |
| layered 1500 x 40, 5 out-edges each | 669 | 560 | 2112 | **284** |
| random unit caps, 1e5 V / 4e5 E | 34.7 | 32.5 | 63.1 | **19.9** |
| random caps in [1, 10], 1e5 V / 4e5 E | **4.3** | 11.8 | 32.7 | 7.0 |

What each Dinic2 ingredient is worth (same families, Dinic2 with the ingredient
removed): 16-byte edges 13-22% on the big families; the multi-push DFS 2x on
layered; killing dead ends (`lvl[to] = -1`) 1.6x on random unit, 10% elsewhere;
`fill` instead of reallocating `lvl`/`ptr` is noise. Capacity scaling (the
`F0R (L, 31)` loop of Dinic.h) is a net loss: it makes Dinic.h 11x slower on the
random sparse family and Dinic2 10x slower, and only wins on capacities so small
that all phases but the last are empty (last row). Looping the BFS edges by
reference instead of by value is noise either way.

## Incremental use (random sparse family)

| operation | Dinic.h | Dinic.h no scaling | Dinic2.h |
|---|---:|---:|---:|
| raise a random edge's cap, then `calc(s, t)` | 466 ms | 14.6 ms | **9.1 ms** |
| lower a cap below its flow with the recipe, then `calc(s, t)` | n/a | n/a | **0.14 ms** |

(from scratch: 471 / 46.5 / 28.7 ms). The raise costs about one BFS over the
part of the residual graph reachable from s; the lower recipe's bounded flows
stop as soon as their few units are found.

## Editing the residual graph between calls

The whole state is the residual capacities; `lvl`/`ptr` are rebuilt every phase,
so nothing persists across calls and no edit can corrupt the next `calc`. For the
pair of an edge, `fwd.c + rev.c = cap + rcap` and the flow on it is
`cap - fwd.c` (`= rev.c` when rcap = 0). An edit is legal when afterwards

1. both residuals of every pair are >= 0,
2. every vertex other than s and t is conserved (flow in = flow out).

Legal edits: raise a cap (`fwd.c += d`); lower a cap to no less than its flow
(`fwd.c -= d`); move flow around a cycle or along an s-t path (add to `fwd.c`,
subtract from `rev.c` on each edge; an s-t path changes the value you track).
Lowering below the flow needs the recipe in the header: put the flow at the new
cap (`fwd.c = 0`, `rev.c -= x`), reroute what you can from a to b
(`calc(a, b, x)`), cancel the rest back to s and from t (`calc(a, s, z)`,
`calc(t, b, z)`, both always succeed: those z units came from s and went to t),
then `calc(s, t)` again because cancelling frees capacity.

Illegal edits do not crash: `calc` returns the s-t augmenting flow it finds from
whatever state it is given, so the total and `left_of_min_cut` are meaningless.

Cost: `calc` from a valid flow keeps every bound it has from scratch (at most V
phases, O(VE) each; on unit networks the residual is still a unit network, so
O(E sqrt V) too), and a call that finds F more units does at most F augmenting
paths, so it is bounded by both. Nothing is amortized across calls: k edits each
followed by `calc` can cost k times a scratch run in the worst case; measured it
is a third of one. A `calc` that finds nothing is one BFS. With capacity scaling
every call is at least 31 BFS rounds, which is why Dinic.h with scaling is 30x
slower per incremental call.
