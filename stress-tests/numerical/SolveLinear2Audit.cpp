// Tests SolveLinear2.h: SolveLinear.h with the two edits from its Description
// applied (Gauss-Jordan elimination + "undefined" for non-unique variables).
// Compares rank / consistency / per-variable determinedness and values against
// exact rational elimination for n<=4, m<=4. written by Claude (audit)
#include "../utilities/template.h"

const db undefined = NAN;  // user-supplied sentinel
using vd = vt<db>;
const db eps = 1e-12;

int solveLinear(vt<vd>& A, vd& b, vd& x) {
    int n = size(A), m = size(x), rank = 0, br, bc;
    if (n) assert(size(A[0]) == m);
    vi col(m); iota(all(col), 0);
    F0R (i, n) {
        db v, bv = 0;
        FOR (r, i, n) FOR (c, i, m)
            if ((v = fabs(A[r][c])) > bv)
                br = r, bc = c, bv = v;
        if (bv <= eps) {
            FOR (j, i, n) if (fabs(b[j]) > eps) return -1;
            break;
        }
        swap(A[i], A[br]);
        swap(b[i], b[br]);
        swap(col[i], col[bc]);
        F0R (j, n) swap(A[j][i], A[j][bc]);
        bv = 1 / A[i][i];
        F0R (j, n) if (j != i) { // SolveLinear2 edit 1
            db fac = A[j][i] * bv;
            b[j] -= fac * b[i];
            FOR (k, i + 1, m) A[j][k] -= fac * A[i][k];
        }
        rank++;
    }
    x.assign(m, undefined); // SolveLinear2 edit 2
    F0R (i, rank) {
        FOR (j, rank, m) if (fabs(A[i][j]) > eps) goto fail;
        x[col[i]] = b[i] / A[i][i];
    fail:; }
    return rank;
}

struct Q { ll p, q; };
Q mk(ll p, ll q = 1) { if (q < 0) p = -p, q = -q; ll g = __gcd(llabs(p), q); if (!g) g = 1; return {p / g, q / g}; }
Q operator-(Q a, Q b) { return mk(a.p * b.q - b.p * a.q, a.q * b.q); }
Q operator*(Q a, Q b) { return mk(a.p * b.p, a.q * b.q); }
Q operator/(Q a, Q b) { return mk(a.p * b.q, a.q * b.p); }
// exact rref of [M | rhs]; returns rank, optionally consistency + a particular solution
int rref(vt<vt<Q>> M, int m, bool* cons, vt<Q>* sol) {
    int n = size(M), r = 0; vi piv;
    F0R (c, m) {
        int p = -1; FOR (i, r, n) if (M[i][c].p) { p = i; break; }
        if (p < 0) continue;
        swap(M[p], M[r]);
        Q iv = M[r][c]; F0R (k, m + 1) M[r][k] = M[r][k] / iv;
        F0R (i, n) if (i != r && M[i][c].p) { Q f = M[i][c]; F0R (k, m + 1) M[i][k] = M[i][k] - f * M[r][k]; }
        piv.pb(c); r++;
    }
    if (cons) {
        *cons = true; FOR (i, r, n) if (M[i][m].p) *cons = false;
        sol->assign(m, mk(0)); F0R (i, r) (*sol)[piv[i]] = M[i][m];
    }
    return r;
}

int main() {
    mt19937 rng(123);
    F0R (it, 100000) {
        int n = rng() % 5, m = rng() % 4 + 1, R = rng() % 3 + 1;
        vt<vt<Q>> M(n, vt<Q>(m + 1));
        vt<vd> A(n, vd(m)); vd b(n), x(m);
        F0R (i, n) {
            F0R (j, m) { ll v = (ll)(rng() % (2 * R + 1)) - R; M[i][j] = mk(v); A[i][j] = v; }
            ll v = (ll)(rng() % (2 * R + 1)) - R; if (rng() % 3 == 0) v = 0; M[i][m] = mk(v); b[i] = v;
        }
        if (n >= 2 && rng() % 2) { // add a multiple of one row to another
            int a = rng() % n, c = rng() % n; ll k = (ll)(rng() % 5) - 2;
            if (a != c) { F0R (j, m + 1) M[a][j] = M[a][j] - mk(-k) * M[c][j];
                F0R (j, m) A[a][j] = (db)M[a][j].p / M[a][j].q; b[a] = (db)M[a][m].p / M[a][m].q; }
        }
        bool cons; vt<Q> sol; int rk = rref(M, m, &cons, &sol);
        int got = solveLinear(A, b, x);
        if (!cons) { assert(got == -1); continue; }
        assert(got == rk);
        F0R (j, m) {
            vt<vt<Q>> M2 = M; M2.pb(vt<Q>(m + 1, mk(0))); M2.back()[j] = mk(1);
            bool det = rref(M2, m, 0, 0) == rk; // x_j unique iff e_j in rowspace(A)
            assert(det == !isnan(x[j]));
            if (det) assert(fabs((db)sol[j].p / sol[j].q - x[j]) < 1e-9);
        }
    }
    cout << "Tests passed!" << endl;
}
