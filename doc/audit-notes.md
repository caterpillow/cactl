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
- **Big blank gaps inside a column / template names orphaned at a column
  bottom** (fixed 2026-08-28). Three independent causes, all in kactlpkg.sty:
  1. multicol's default `\flushcolumns` stretches every column to full height.
     Our vertical list has almost no stretchable glue (listings zero
     `\parskip` in an Init hook, lstmisc.sty:1296; captions run after
     `\@parboxrestore`; `\myneedspace` is a fixed `\vskip`), so a column's
     slack was dumped into the *one* glue that had any stretch: the `parskip` (plus 2pt) before a template name → a
     third-of-a-column hole between two templates (p13 BerlekampMassey →
     LinearRecurrence). Now `\raggedcolumns`: slack sits at column bottoms.
  2. `\@makechapterhead` was copied from report.cls without the enclosing
     braces, so `\interlinepenalty\@M` leaked into the whole document and no
     paragraph could ever break across columns (17-line chapter prose +
     section heading = 21 unbreakable lines → 17 lines of slack). The leak is
     removed; descriptions stay atomic via `\interlinepenalty\@M` inside
     `\@makecaption`, and wrapped code lines via an lst `Init` hook.
  3. listings' `\lst@Init` emits `\par\penalty-50 \vspace{aboveskip}` and
     `\lst@MakeCaption t` starts with `\allowbreak` — two legal breakpoints
     between the name line and its caption (in a stretch-free column every
     fitting break costs the same, TeX takes the *last* one that fits);
     `\myneedspace{3\baselineskip}` only reserves name + 2 lines, so a tall
     (atomic) description pushed the caption to the next column and left
     the name behind (p30 lineIntersection.h; also IntegrateAdaptive, 2sat,
     AhoCorasick). Both are `\patchcmd`'ed to `\nobreak`, and
     `\kactlimport` skips its own needspace/`\penalty-100` when `\if@nobreak`
     (directly after a heading) so a section title can't be orphaned either.
  Follow-up (same day): a heading's `\penalty-300` (and listings' -50, our
  -100) could still beat "fill the column" whenever the page had stretch
  (Absorption pushed to a new page above a gap), so all vertical stretch is
  stripped (parskip, display and heading skips) — with none, TeX takes the
  last break that fits — and club/widow penalties are 10000. A caption is
  glued to its first three code lines (`\lst@frameInit` patch + `Init` and
  `EveryLine` hooks), and a chapter head to its first template (`\nobreak`
  after `\startcontents`, `\kactlimport` emits `\nobreak` when `@nobreak`).
  Diagnostics that worked: `pdftoppm -r 80 -f N -l N -png`, then vary one
  thing per scratch copy (`make fast` is ~6 s). **Caveat:** the TOC is
  typeset inside the multicols, so a scratch copy built in an empty `build/`
  (no `kactl.toc`) has a different layout from the real PDF — copy
  `build/kactl.{aux,toc,ptc,out}` first or use `make kactl`. `\showbox` of
  each column inside `\multi@column@out` shows exactly which glue absorbed
  the slack (multicol sets `\vbadness=10001`, so there are no Underfull
  warnings to grep for).

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
-.3ex` (negative overall = |values| + no indent). Chapter heads got a
`\myneedspace{7\baselineskip}`; the per-template needspace stays at 3 lines —
raising it to 5 made multicol stretch the columns into large gaps between
templates, which is worse than an occasional stranded heading.
