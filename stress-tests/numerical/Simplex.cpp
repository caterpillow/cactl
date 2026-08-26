// Tests Simplex.h: random 2-var LPs vs exact rational vertex enumeration,
// checking feasible optima, -inf on infeasible, +inf on unbounded.
// Also compiled with -DNDEBUG (old assert(simplex(2)) bug only bit there).
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/numerical/Simplex.h"
#undef NDEBUG // header compiled with whatever -D was given; test checks always fire
#include <cassert>

mt19937 rng(1234);
int ri(int lo, int hi) { return (int)(rng() % (hi - lo + 1)) + lo; }

int main() {
	int nfeas = 0, ninf = 0, nunb = 0;
	F0R (it, 5000) {
		int m = ri(0, 4), n = 2;
		// constraints: rows of full system a.x <= b: m random + x>=0 as -x_i <= 0
		vt<array<ll,3>> C; // {a0, a1, b}
		vt<vd> A(m, vd(n)); vd b(m), c(n);
		F0R (i, m) {
			ll a0 = ri(-4, 4), a1 = ri(-4, 4), bb = ri(-4, 4);
			A[i] = {(db)a0, (db)a1}; b[i] = (db)bb;
			C.pb({a0, a1, bb});
		}
		C.pb({-1, 0, 0}); C.pb({0, -1, 0});
		ll c0 = ri(-4, 4), c1 = ri(-4, 4);
		c = {(db)c0, (db)c1};
		// exact brute: enumerate vertices = intersections of constraint pairs
		bool feas = false, unb = false;
		db best = -linf;
		int K = size(C);
		F0R (i, K) FOR (j, i + 1, K) {
			ll q = C[i][0]*C[j][1] - C[i][1]*C[j][0];
			if (!q) continue;
			ll px = C[i][2]*C[j][1] - C[i][1]*C[j][2]; // x = px/q
			ll py = C[i][0]*C[j][2] - C[i][2]*C[j][0];
			bool ok = true;
			for (auto& r : C) { // r0*px + r1*py <= r2*q (sign of q)
				ll lhs = r[0]*px + r[1]*py, rhs = r[2]*q;
				if (q > 0 ? lhs > rhs : lhs < rhs) { ok = false; break; }
			}
			if (!ok) continue;
			feas = true;
			best = max(best, (db)(c0*px + c1*py) / q);
		}
		if (feas) {
			// unbounded iff extreme ray d of {d>=0, Ad<=0} has c.d > 0
			for (auto& r : C) for (int sg : {1, -1}) {
				ll dx = sg*r[1], dy = -sg*r[0];
				if (dx < 0 || dy < 0 || (!dx && !dy)) continue;
				bool inC = true;
				F0R (i2, m) if (C[i2][0]*dx + C[i2][1]*dy > 0) inC = false;
				if (inC && c0*dx + c1*dy > 0) unb = true;
			}
		}
		vd x;
		db val = LPSolver(A, b, c).solve(x);
		if (!feas) { assert(val == -linf); ninf++; }
		else if (unb) { assert(val == linf); nunb++; }
		else {
			assert(isfinite(val) && abs(val - best) < 1e-6);
			assert(size(x) == n && x[0] > -1e-7 && x[1] > -1e-7);
			F0R (i, m) assert(A[i][0]*x[0] + A[i][1]*x[1] < b[i] + 1e-6);
			assert(abs(c[0]*x[0] + c[1]*x[1] - best) < 1e-6);
			nfeas++;
		}
	}
	assert(nfeas > 500 && ninf > 100 && nunb > 100);
	cout << "Tests passed! feas=" << nfeas << " inf=" << ninf << " unb=" << nunb << endl;
}
