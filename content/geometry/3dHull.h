/**
 * Author: Johan Sannemo
 * Date: 2017-04-18
 * Source: https://codeforces.com/blog/entry/81768
 * Description: Incremental 3D hull. The first four points must be noncoplanar.
 *  Returns outward triangles with original indices; later coplanarity and
 *  duplicates are allowed. May retain redundant boundary vertices.
 *  \texttt{hull3d\_vertices} returns only extreme vertex indices; faces are unchanged.
 *  Requires reliable orientation signs; use a sufficiently wide integer type
 *  for exact integer inputs. Floating-point roundoff can affect degeneracies.
 * Time: Hull: O(n^2) time and space. Vertices: O(n + m\log m), m = number of triangles.
 * Status: stress-tested
 */
#pragma once

#include "Point3D.h"

using P3 = Point3D<db>;
struct F { P3 q; int a, b, c; };

vt<F> hull3d(const vt<P3>& A) {
    assert(size(A) >= 4 && (A[1] - A[0]).cross(A[2] - A[0]).dot(A[3] - A[0]) != 0);
    vt<vt<bool>> dead(size(A), vt<bool>(size(A), true));
    vt<F> FS;
    auto add = [&](int a, int b, int c) {
        FS.pb({(A[b] - A[a]).cross(A[c] - A[a]), a, b, c});
        dead[a][b] = dead[b][c] = dead[c][a] = false;
    };
    add(0, 1, 2); add(0, 2, 1); // Two-sided disk; point 3 makes a tetrahedron.
    FOR (i, 3, size(A)) {
        vt<F> keep;
        for (F f : FS) {
            if (f.q.dot(A[i] - A[f.a]) > 0)
                dead[f.a][f.b] = dead[f.b][f.c] = dead[f.c][f.a] = true;
            else keep.pb(f);
        }
        FS.clear();
        for (F f : keep) {
            int v[] = {f.a, f.b, f.c};
            F0R (j, 3) {
                int a = v[j], b = v[(j + 1) % 3];
                if (dead[b][a]) add(b, a, i);
            }
        }
        FS.insert(FS.end(), all(keep));
    }
    return FS;
} // <hash>

// For a full-dimensional hull from hull3d. Does not modify its triangulation.
vt<int> hull3d_vertices(const vt<P3>& A, const vt<F>& faces) {
    map<pair<int, int>, int> edges;
    vt<int> neighbor(size(A), -1), corner(size(A)), ans;
    for (F f : faces) {
        int v[] = {f.a, f.b, f.c};
        F0R (j, 3) {
            int a = v[j], b = v[(j + 1) % 3], c = v[(j + 2) % 3];
            auto [it, added] = edges.emplace(minmax(a, b), c);
            if (added || f.q.dot(A[it->second] - A[a]) == 0) continue;
            F0R (k, 2) {
                if (neighbor[a] < 0) neighbor[a] = b;
                else if (!((A[neighbor[a]] - A[a]).cross(A[b] - A[a]) == P3()))
                    corner[a] = 1;
                swap(a, b);
            }
        }
    }
    F0R (i, size(A)) if (corner[i]) ans.pb(i);
    return ans;
}
