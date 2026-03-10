/**
 * Author: caterpillow
 * Date: 2025-09-01
 * License: CC0
 * Source: forgor
 * Description: First, use BiconnectedComponents to locate VERTEX components (including degree 0 nodes).
 *  To build a block cut tree, make a bipartite graph:
 *  Put all the normal nodes on the left, and make a new node for each bcc on the right.
 *  Draw edges from normal nodes to BCC that contain them. 
 *  Note that the graph may be disconnected.
 * Status: true
 */