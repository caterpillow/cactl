// Tests Newton.h: roots of assorted functions vs closed forms, a multiple
// root, a case where raw Newton would leave the bracket, and minimising a
// convex function via its derivative. Written by Claude (audit).
#include "../utilities/template.h"
#include "../../content/numerical/Newton.h"
int main() {
	assert(abs(newton(0, 2, [](db x) { return x * x - 2; }, [](db x) { return 2 * x; }) - sqrt(2)) < 1e-12);
	assert(abs(newton(0, 1, [](db x) { return cos(x) - x; }, [](db x) { return -sin(x) - 1; }) - 0.7390851332151607) < 1e-12);
	assert(abs(newton(-1, 2, [](db x) { return x * x * x; }, [](db x) { return 3 * x * x; })) < 1e-6); // triple root
	// f = atan(x) from a far bracket: unsafeguarded Newton diverges from x=2
	assert(abs(newton(-5, 3, [](db x) { return atan(x); }, [](db x) { return 1 / (1 + x * x); })) < 1e-12);
	// minimise (x - 3)^4 + x^2 by Newton on the derivative
	db m = newton(-10, 10, [](db x) { return 4 * pow(x - 3, 3) + 2 * x; }, [](db x) { return 12 * (x - 3) * (x - 3) + 2; });
	assert(abs(4 * pow(m - 3, 3) + 2 * m) < 1e-9);
	puts("Tests passed!");
}
