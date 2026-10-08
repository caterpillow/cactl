/**
 * Author: caterpillow
 * Date: 2025-09-01
 * License: CC0
 * Source: forgor
 * Description: Run BiconnectedComponents with the bridge line in \texttt{init} uncommented (bridges become 1-edge blocks) to locate VERTEX components. A block's vertices are the endpoints of its edges. Degree 0 nodes have no block.
 *  To build a block cut tree, make a bipartite graph:
 *  Put all the normal nodes on the left, and make a new node for each bcc on the right.
 *  Draw edges from normal nodes to BCC that contain them. 
 *  Note that the graph may be disconnected.
 * Status: true
 */
// E = edges, b = BCC after b.init(n, E), bridge line on
vt<vi> T(n + size(b.comps)); // block c is node n + c
vi mk(n, -1);
F0R (c, size(b.comps)) for (int e : b.comps[c])
    for (int x : {E[e].f, E[e].s})
        if (mk[x] != c) mk[x] = c, T[x].pb(n + c), T[n + c].pb(x);