# Notebook conventions & landmines (2026-08 audit notes)

Durable notes from the full-notebook audit + fix pass. Written by Claude; kept
because none of this is obvious from the code alone.

## Template gotchas (template.cpp)

- `#define size(x) ((int)(x).size())` is a *function-like macro*: member calls
  `v.size()` do **not** compile — always write `size(v)`. It also corrupts any
  header that declares a `size()` member, notably `<bits/extc++.h>` (pb_ds).
  **Any `#include <bits/extc++.h>` must go above the template**, or be wrapped
  in `#undef size` / re-`#define`. Affected: HashMap, OrderStatisticTree,
  MinCostMaxFlow, SimAnneal.
- `#define f first` / `#define s second` break any declaration named `f` or `s`.
- `pi` is now `pair<int, int>` — that's why KMP's prefix function is named
  `pfun` (a function named `pi` no longer compiles).
- Conventions: `inf` = int 1e9, `INF` = ll 1e18, `db` = double. Headers use the
  template's constants; don't re-declare them locally at global scope
  (redefinition error) — BellmanFord/FloydWarshall/Simplex were converted.

## Prefix-hash markers

- Writing `// <hash>` on a line of a C++/Java template makes the PDF print
  `// abc123` — the hash of the *printed listing* up to and including that line,
  using the hash.sh pipeline (comments and whitespace never affect hashes, so
  the marker line itself is neutral and the printed value is reproducible:
  type up to the marker, select, `:Hash`).
- Multiple markers per file are fine. Currently used in
  NumberTheoreticTransform.h.

## Rendering pipeline

- `header.tmp` (the page-header state file) lives in the **repo root** (cwd of
  pdflatex), not `build/`. The cleanup hooks now remove the right file; if
  headers ever look wrong again, delete a stale `header.tmp` and rebuild.
- Page-header file lists longer than ~110 chars are shrunk with `\resizebox`
  so they can't collide with the team name (the old `\fontsize{8}{8}` had no
  `\selectfont` and never actually did anything).
- The tiny "hash, N lines" caption shares the Description/Time line only when
  it fits, else wraps to its own line (`\@makecaption` in kactlpkg.sty).
- Preprocessor rules worth remembering: comment blocks must contain `Author:`
  and `Description:`; any other `Capitalized:` word at the start of a `* ` line
  is an *unknown command* → **build error** (this bit us with "Bounds:").
  Bare math like `\log` in a Description needs `$...$` (only Time/Memory get
  auto-`\bigo`).

## Known landmines left in place (deliberate)

- **NumberTheoreticTransform.h** inherits `mod = 1e9+7` from ModPow.h but
  `root = 62` is for **998244353**. You *must* change ModPow's `mod` to
  998244353 (or define mod before including) or every convolution is silently
  garbage. WildcardPatternMatching sidesteps this by using FFTMod's
  `convMod<(119<<23)+1>`.
- SegmentTree.h expects *you* to define `struct T` (associative `operator+`)
  and `const T id` above it. StaticRangeQuery/SparseTable use `#define comb/func`.
- VirtualTree.h needs globals `pos` (dfs entry times) and a callable
  `lca(u, v)`. DEsopoPape.h needs a global `int n`.
- ModInverse.h and MinCostMaxFlow.h have `include-line` comments: they print
  uncommented in the PDF but the raw files intentionally don't compile alone.
- Paste-into-main snippets (never compile at namespace scope): Sieve.h,
  CentroidDecomp.h, AllPointPairs.h, STL.h, MoQueries.h (`{ ... }` stubs).
- AllPointPairs.h requires Point's **radial** `operator<` (commented block in
  Point.h) and distinct points. The live loop assumes no three collinear
  points (one adjacent swap per event); with collinear points several events
  share a direction and the correct move is reversing each collinear block,
  which the commented variant in the same file does.
- Simplex returns IEEE `-linf/linf` for infeasible/unbounded — a legitimate
  optimum can't be confused with them anymore (old code used ±1e9).
- template.cpp's `cin.exceptions(cin.failbit)` makes the read-until-EOF idiom
  `while (cin >> x)` throw instead of ending the loop — catch or avoid.

## Misc

- Stress harness: `stress-tests/utilities/template.h` now mirrors the contest
  template plus legacy `rep/sz/pii` shims; tests compile from the repo root.

## "This subsection bricks the latex" — root cause (fixed 2026-08-27)

`\subsection`/`\subsubsection` in kactlpkg.sty had beforeskip `1ex plus -1ex
minus -.3ex`. Standard LaTeX writes the *whole* skip negative (`-3.25ex plus
-1ex ...`) and takes absolute values, so the stretch is really +1ex; here the
overall skip was positive, so the stretch was literally **negative**. Whenever
multicol had to stretch a short column to the flush bottom, every heading in
that column moved *backwards* by the stretch ratio and overprinted the text
above it — which only happened for certain selections of content (hence
"different text regions"). Fixed by making the skip `-1ex plus -1ex minus
-.3ex` (negative overall = |values| + no indent). `\myneedspace` was also
raised to 5 lines (7 for chapter heads) so headings travel with their code.
