// Tests PolyInterpolate.h: round-trip — random poly of degree <= 7 evaluated
// at distinct points, interpolate() must recover the coefficients.
// written by Claude (audit)
#include "../utilities/template.h"
#include "../../content/numerical/PolyInterpolate.h"

mt19937 rng(31337);

int main() {
	F0R (it, 20000) {
		int n = rng() % 8 + 1; // n coefficients, degree <= 7
		vd coef(n);
		for (db& c : coef) c = (db)((int)(rng() % 21) - 10);
		vi xs { -8,-7,-6,-5,-4,-3,-2,-1,0,1,2,3,4,5,6,7,8 };
		shuffle(all(xs), rng);
		vd x(n), y(n);
		F0R (i, n) {
			x[i] = xs[i];
			db v = 0;
			for (int k = n; k--;) v = v * x[i] + coef[k];
			y[i] = v;
		}
		vd res = interpolate(x, y, n);
		assert(size(res) == n);
		F0R (i, n) assert(abs(res[i] - coef[i]) < 1e-6);
	}
	cout << "Tests passed!" << endl;
}
