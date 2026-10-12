# Exponential multiset hashing

`ExpHash<K>` represents `sum(B^x) mod P`, where `P = 2^61-1` and
`B = 37^K mod P`. The constructor represents a residue; `point(x)`
represents the singleton object `{x}`. Addition/subtraction changes
multiplicities, multiplication convolves them (all pairwise **sums**),
and multiplying by `point(t)` translates every object by `t`.

Use `ll(1e11) + {3, 7, 13, 19, 21, 27}` as convenient choices of `K` or
coordinate coefficients. All six are coprime to **P-1**; they need not be
prime. For two fingerprints, choose separate K's and separate coefficients.
These are fixed-data heuristics, not uniform random samples from the full
parameter space. Merely choosing among six constants does not give the
probability bound below.

## Degree and collision probability

For fixed multisets S and T, write their difference polynomial:

`F(t) = sum_x (count_S(x) - count_T(x)) t^x`.

Cancel zero coefficients modulo P, then subtract the smallest remaining
exponent from every exponent. This multiplies F by a nonzero factor when
evaluated at B, so it preserves collisions. In 1D its degree is now
`D = largest exponent - smallest exponent`. For example, `{10,14}` versus
`{11,12}` gives `t^10(1+t^4-t-t^2)`, so D=4, not 14 and not the set size.

A nonzero degree-D polynomial has at most D roots. A uniform primitive
root is chosen from `G = phi(P-1) = 406467072000000000` possibilities,
so a fixed comparison collides with probability at most `min(1,D/G)`.
If all coefficients cancel modulo P, the collision is certain. Exponents
wrap modulo P-1, so this is not an injective encoding of arbitrary integers.

For vectors use a product of coordinate bases, e.g. `U^x V^y`.
After subtracting the minimum exponent in each coordinate, total degree
is the largest **sum** of exponents in any remaining monomial; it is at
most the sum of coordinate spans. Independent uniform primitive coordinate
bases give the same D/G bound by Schwartz-Zippel. A uniform primitive B
and independent uniform unit coefficients in `B^(x+c*y+...)` give these
independent coordinate bases.
With fixed c and only random B, that multivariate argument does not apply:
use the degree of the packed univariate polynomial instead.

For Q fixed comparisons each bounded by epsilon, a union bound gives
`P(any false equality) <= min(1,Q*epsilon)`. Two independent fingerprints
give `min(1,Q*epsilon^2)`. For all pairs among n items, use `Q=n*(n-1)/2`.
The comparisons share parameters; do not assume independent collision
events or an exact birthday distribution. These are worst-case bounds,
not predictions that every comparison fails with probability D/G.
For D=1e6 and Q=1e6, the bounds are about 2.46e-6 (single) and
6.05e-18 (double), provided the parameters were uniformly sampled as above.

## Fixed-base exponentiation

Write `e = sum_j d_j 2^(L*j)` and store
`pw[j][d] = B^(d*2^(L*j)) mod P`. Then multiply the selected entries.
`FixedModPow<P>` uses L=8: eight tables of 256 residues, 16 KiB per base,
seven multiplications per query. L=16 uses four tables of 65536 residues,
2 MiB per base and three multiplications. Tables are shared by every
hash instance of the same K. Each hash itself remains one 64-bit integer.
For repeated ranges of one step d, also cache `(B^d-1)^(-1)` when nonzero.

## Measurements (2026-10-09)

GCC 13.3.0, `-std=c++17 -O2`, AMD Ryzen 9 9950X3D, pinned to CPU 0.
Medians of seven shuffled repetitions; inputs are prepared before timing.
The harness checks checksums against the other implementations, uses
non-inlined functions to keep the runtime modulus opaque, and consumes
results. Multiplication chains use units modulo M to avoid collapsing to
zero for composite M. Runtime M is fixed throughout each loop, so the
floating reciprocal can be hoisted. Numbers are workload-specific.

Nanoseconds per modular multiplication for P=2^61-1:

| Implementation | Runtime P, chain | Runtime P, 8 chains | Constant P, chain | Constant P, 8 chains |
|---|---:|---:|---:|---:|
| ModMulLL long double | 8.63 | 1.66 | 8.47 | 1.75 |
| unsigned __int128 | 4.28 | 2.48 | 3.73 | 1.12 |
| signed __int128 | 4.26 | 2.48 | 3.73 | 1.12 |
| explicit Mersenne fold | 1.68 | 0.77 | 1.67 | 0.78 |

The Mersenne routine always knows P; it is a specialized reference, not
a variable-modulus implementation. For canonical a,b<P, it computes
`z=(__uint128_t)a*b; r=(z&P)+(z>>61);` then subtracts P if needed.
`ExpHash::operator*` uses this fold (`ExpHash::mul`); the `FixedModPow`
table still uses the constant-modulus __int128 expression.
Assembly inspection shows a `__umodti3` call for runtime __int128 remainder,
but inline reduction for constant P. Constant P removes the floating
division from ModMulLL, but floating conversion and correction remain.
Thus the previous unconditional "2x faster" claim is not supported.
Earlier benchmark builds also showed an unexplained ~81 ns long-double
constant-P chain result; the table is from the final harness. No claim is
made that these numbers generalize across builds, compilers or CPUs.

Nanoseconds per addition and equality check, with canonical or signed
representatives. The mixed column also multiplies once per eight iterations:

| Representation/update | Add + equality | Mixed |
|---|---:|---:|
| [0,P), conditional correction | 0.75 | 1.22 |
| [0,P), old double remainder | 4.28 | 4.76 |
| (-P,P), conditional correction | 0.95 | 1.62 |
| (-P,P), signed remainder | 2.07 | 2.57 |

For signed representatives, equality was `(a-b)%P==0`. This is safe for
this P, but costs more than comparing canonical residues. GCC optimized
this particular equality without a division instruction. Negative residues
also require signed multiplication or normalization before unsigned mmul.
The revised hash uses canonical residues, `if (x >= P) x -= P` for addition
(GCC emits cmov) and branchless `x += x >> 63 & P` for subtraction: GCC
compiles `if (x < 0) x += P` to a branch that mispredicts ~50% on hashes.

Nanoseconds per power query for 61-bit exponents, warm tables:

| Method | ns/query |
|---|---:|
| Binary, ModMulLL | 521 |
| Binary, constant __int128 | 246 |
| Binary, explicit Mersenne fold | 184 |
| Table L=8 | 19.9 |
| Table L=16 | 9.6 |

Single-run table construction measurements: about 14 us / 1.52 ms for
L=8 / L=16. Cache behavior and query count determine whether L=16 is useful.

To reproduce from the repository root:

```sh
g++ -std=c++17 -O2 -Wall -Wextra stress-tests/data-structures/helpers/ExponentialHashBench.cpp -o /tmp/exphash-bench
/tmp/exphash-bench
```

Source for the multivariate bound:
[CMU 15-451 polynomial hashing notes](https://www.cs.cmu.edu/~15451-f20/LectureNotes/hashing-2.pdf).
