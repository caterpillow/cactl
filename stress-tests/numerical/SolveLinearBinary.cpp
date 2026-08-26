// Tests SolveLinearBinary.h (the REAL header, not a copy): exhaustive n,m<=4
// vs solution enumeration, plus 20k random systems (incl. infeasible /
// underdetermined) vs independent F2 elimination. Run under ASan.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/numerical/SolveLinearBinary.h"

mt19937 rng(777);

// independent rank of an n x m F2 matrix given as row masks
int f2rank(vt<unsigned>& rows, int m) {
	int r = 0;
	F0R (c, m) {
		int p = -1;
		FOR (i, r, size(rows)) if (rows[i] >> c & 1) { p = i; break; }
		if (p < 0) continue;
		swap(rows[r], rows[p]);
		F0R (i, size(rows)) if (i != r && (rows[i] >> c & 1)) rows[i] ^= rows[r];
		r++;
	}
	return r;
}

void runCase(int n, int m, vt<unsigned>& rowA, vt<int>& bv) {
	vt<bs> A(n); vi b(n); bs x;
	F0R (i, n) { F0R (j, m) A[i][j] = rowA[i] >> j & 1; b[i] = bv[i]; }
	int r = solveLinear(A, b, x, m);
	vt<unsigned> Ab(n), Ao(n);
	F0R (i, n) { Ao[i] = rowA[i]; Ab[i] = rowA[i] | (unsigned)bv[i] << m; }
	int rA = f2rank(Ao, m), rAb = f2rank(Ab, m + 1);
	if (rA != rAb) { assert(r == -1); return; }
	assert(r == rA);
	F0R (i, n) { // check A x == b
		int v = 0;
		F0R (j, m) v ^= (rowA[i] >> j & 1) & (int)x[j];
		assert(v == bv[i]);
	}
}

int main() {
	// exhaustive small systems
	F0R (n, 4) F0R (m, 5) if (n * m <= 12) {
		vt<unsigned> rowA(n); vt<int> bv(n);
		F0R (mask, 1 << (n * m)) F0R (bm, 1 << n) {
			F0R (i, n) rowA[i] = (mask >> (i * m)) & ((1u << m) - 1), bv[i] = bm >> i & 1;
			runCase(n, m, rowA, bv);
		}
	}
	// 20k random systems, up to 10x12, biased toward singular/infeasible
	F0R (it, 20000) {
		int n = rng() % 10 + 1, m = rng() % 12 + 1;
		vt<unsigned> rowA(n); vt<int> bv(n);
		F0R (i, n) rowA[i] = rng() & ((1u << m) - 1), bv[i] = rng() & 1;
		if (n > 1 && rng() % 3 == 0) { // force dependent row, maybe inconsistent
			int i = rng() % n, j = rng() % n;
			if (i != j) { rowA[i] = rowA[j]; bv[i] = rng() & 1 ? bv[j] : bv[j] ^ 1; }
		}
		runCase(n, m, rowA, bv);
	}
	cout << "Tests passed!" << endl;
}
