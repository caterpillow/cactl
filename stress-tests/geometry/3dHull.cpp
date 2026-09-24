// Compare the incremental mesh and its extreme vertices with independently
// enumerated supporting planes and strict 2D hulls of their coplanar points.
// Small integer coordinates keep all predicates exact, including with db=double.
#include "../utilities/template.h"
#include "../../content/geometry/3dHull.h"

using IP = Point3D<ll>;
ll orient(IP a, IP b, IP c, IP d) {
    return (b - a).cross(c - a).dot(d - a);
}

bool initialize(vt<IP>& p) {
    // The routine requires the first four points to form a tetrahedron.
    FOR(i, 1, 4) {
        int j = i;
        for (; j < size(p); j++) {
            if (i == 1 && !(p[j] == p[0])) break;
            if (i == 2 && !((p[1] - p[0]).cross(p[j] - p[0]) == IP())) break;
            if (i == 3 && orient(p[0], p[1], p[2], p[j])) break;
        }
        if (j == size(p)) return false;
        swap(p[i], p[j]);
    }
    return true;
}

pair<ll, set<IP>> oracle(const vt<IP>& p) {
    set<array<ll, 4>> planes;
    int n = size(p);
    F0R(i, n) FOR(j, i + 1, n) FOR(k, j + 1, n) {
        IP q = (p[j] - p[i]).cross(p[k] - p[i]);
        if (q == IP()) continue;
        bool pos = false, neg = false;
        for (IP a : p) {
            ll sgn = q.dot(a - p[i]);
            pos |= sgn > 0; neg |= sgn < 0;
        }
        if (pos == neg) continue;
        if (pos) q = q * -1;
        ll d = q.dot(p[i]);
        ll g = gcd(gcd(abs(q.x), abs(q.y)), gcd(abs(q.z), abs(d)));
        planes.insert({q.x / g, q.y / g, q.z / g, d / g});
    }
    ll volume6 = 0;
    set<IP> vertices;
    for (auto pl : planes) {
        IP q(pl[0], pl[1], pl[2]);
        int drop = q.x ? 0 : q.y ? 1 : 2;
        vt<array<ll, 3>> a, h;
        F0R(i, n) if (q.dot(p[i]) == pl[3]) {
            if (drop == 0) a.pb({p[i].y, p[i].z, i});
            if (drop == 1) a.pb({p[i].x, p[i].z, i});
            if (drop == 2) a.pb({p[i].x, p[i].y, i});
        }
        sort(all(a));
        a.erase(unique(all(a), [](auto u, auto v) {
            return u[0] == v[0] && u[1] == v[1];
        }), a.end());
        auto cross = [](auto u, auto v, auto w) {
            return (v[0] - u[0]) * (w[1] - u[1])
                 - (v[1] - u[1]) * (w[0] - u[0]);
        };
        for (auto u : a) {
            while (size(h) > 1 && cross(h[size(h) - 2], h.back(), u) <= 0)
                h.pop_back();
            h.pb(u);
        }
        int cut = size(h);
        ROF(j, 0, size(a) - 1) {
            auto u = a[j];
            while (size(h) > cut && cross(h[size(h) - 2], h.back(), u) <= 0)
                h.pop_back();
            h.pb(u);
        }
        h.pop_back();
        for (auto u : h) vertices.insert(p[u[2]]);
        FOR(j, 1, size(h) - 1) {
            IP u = p[h[0][2]], v = p[h[j][2]], w = p[h[j + 1][2]];
            if ((v - u).cross(w - u).dot(q) < 0) swap(v, w);
            volume6 += u.dot(v.cross(w));
        }
    }
    return {volume6, vertices};
}

int checked = 0, cleaned = 0;
void check(vt<IP> p) {
    if (!initialize(p)) return; // Lower-dimensional inputs are outside the contract.
    checked++;
    vt<P3> a;
    for (IP u : p) a.pb(P3(u.x, u.y, u.z));
    vt<F> faces = hull3d(a);
    auto [expected_volume, expected_vertices] = oracle(p);
    assert(expected_volume > 0);
    ll volume = 0;
    map<pi, int> edges;
    set<int> used;
    for (F t : faces) {
        int ids[] = {t.a, t.b, t.c};
        for (int i : ids) assert(0 <= i && i < size(p));
        IP q = (p[t.b] - p[t.a]).cross(p[t.c] - p[t.a]);
        assert(!(q == IP()));
        assert(t.q == P3(q.x, q.y, q.z));
        for (IP u : p) assert(q.dot(u - p[t.a]) <= 0);
        volume += p[t.a].dot(p[t.b].cross(p[t.c]));
        F0R(j, 3) {
            used.insert(ids[j]);
            edges[{ids[j], ids[(j + 1) % 3]}]++;
        }
    }
    for (auto [e, count] : edges)
        assert((count == 1 && edges.at({e.second, e.first}) == 1));
    assert(size(used) - size(edges) / 2 + size(faces) == 2);
    assert(volume == expected_volume);
    vi ids = hull3d_vertices(a, faces);
    set<IP> got;
    for (int i : ids) {
        assert(used.count(i));
        assert(got.insert(p[i]).second); // No duplicate coordinates in the result.
    }
    assert(got == expected_vertices);
    cleaned += size(ids) < size(used);
}

int main() {
    // The origin becomes respectively an edge-interior and a face-interior point.
    check({IP(0, 0, 0), IP(1, 0, 0), IP(0, 1, 0), IP(0, 0, 1), IP(-1, 0, 0)});
    check({IP(0, 0, 0), IP(2, 0, 0), IP(0, 2, 0), IP(0, 0, 2), IP(-2, -2, 0)});
    // Regression: orienting a new face using a coplanar surviving face's vertex.
    check({IP(0, 0, 0), IP(4, 0, 0), IP(0, 4, 0), IP(0, 0, 4),
           IP(-4, 4, 0), IP(0, 1, 1)});
    // Interior, face, edge and duplicate points of an unchanged tetrahedron.
    check({IP(0, 0, 0), IP(6, 0, 0), IP(0, 6, 0), IP(0, 0, 6),
           IP(1, 1, 1), IP(1, 1, 0), IP(3, 0, 0), IP(0, 0, 0), IP(6, 0, 0)});
    mt19937 rng(91327);
    F0R(it, 3000) {
        int n = rng() % 13 + 4, radius = it % 3 == 0 ? 1 : 2;
        auto rnd = [&] { return (int) (rng() % (2 * radius + 1)) - radius; };
        vt<IP> p;
        F0R(i, n) p.pb(IP(rnd(), rnd(), rnd()));
        check(p);
    }
    F0R(it, 200) {
        vt<IP> p;
        FOR(x, -1, 2) FOR(y, -1, 2) FOR(z, -1, 2) p.pb(IP(x, y, z));
        F0R(i, 3) p.pb(p[rng() % size(p)]);
        shuffle(all(p), rng);
        check(p);
    }
    assert(cleaned > 0);
    cout << "Tests passed! " << checked << " full-dimensional sets.\n";
}
