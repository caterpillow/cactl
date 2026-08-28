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
- Multiple markers per file are fine. 27 headers carry 81 of them:
  `grep -rln '<hash>' content/`.

## Rendering pipeline

### One template

The preprocessor (`content/tex/preprocessor.py`) turns a source file into four
plain-LaTeX lines, all defined in `kactlpkg.sty`:

    \kactlname{Foo.h} \kactlref{Foo.h} \kactlheader{block}{includes}{hash, N lines}
    \begin{lstlisting}[language=C++] ... \end{lstlisting}

- `\kactlname` is the large name line (its `\par` sits *outside* the group, or
  `\large` would set the line's leading).
- `\kactlref` records the name for the page header, see below.
- `\kactlheader` sets the `\textbf{Description:} ... \\ \textbf{Time:} ...`
  block — already escaped by the preprocessor — and ends it with the tiny
  `includes  hash, N lines` caption, which shares the block's last line when
  it fits and takes its own right-aligned line when it does not. The block is
  set inside `\@parboxrestore`, i.e. justified with `\\` breaking a line,
  which is the one place in the document that is *not* ragged-right. A raw
  listing (`-l raw`, no doc comment) passes an empty block and only gets the
  caption.
- **`emit_listing()` is the seam.** Everything above it is renderer-agnostic
  LaTeX; only that one function decides how the code itself is typeset, so
  swapping listings for something else (minted, a custom box) is a
  one-function change. Nothing rides on listings' caption machinery any more —
  the old design fed our block through `caption=`, which is why the `.sty`
  still patches `\lst@Init`/`\lst@MakeCaption`/`\lst@frameInit` (see the
  column-filling invariants below).

### Page headers

- `\kactlref` places `\marks\hdrmark{name}` after each name line. TeX ships
  pages asynchronously (very much so under multicol) and `\botmarks` can
  already point into material pushed to the *next* page, so `kactlpkg.sty`
  hooks multicol's `\set@keptmarks` (run right after every column `\vsplit`)
  and keeps the last `\hdrmark` any column of the current page contained;
  `\@outputpage` clears it once the header is typeset.
- `\chead` then shells out to `preprocessor.py --print-header "<mark>"`, which
  prints every not-yet-printed name up to that one and drops them from its
  state file. That state file is `header.tmp` in the **repo root** (the cwd of
  pdflatex), not `build/`; it is removed at `\begin{document}` and
  `\end{document}`. Running the preprocessor by hand appends to it.
- A list longer than 110 characters is set in a `\parbox` of
  `\headwidth-10cm`, centred, `\footnotesize`, names joined with
  `\hspace{.5em}`; shorter ones are a single line joined with `\enspace`. The
  separator must be breakable (`\enspace` is a kern and never breaks), and the
  parbox must stay narrower than `\headwidth` so it cannot collide with the
  team name or the page number. The header box is 25pt high and
  bottom-aligned, so the second line grows *upward*, towards the paper edge.

### Geometry

- `geometry` is loaded *with* `headheight=25pt,headsep=2pt`, so it computes
  `\topmargin` from them. Setting `\headheight`/`\headsep` afterwards leaves
  the layout stale until the next `\geometry` call, and `test-session.tex`
  makes none.
- `\kactlmargins{l}{r}{t}{b}` (defaults 0.5cm 0.5cm 1.1cm 0.4cm) re-runs
  `\geometry` and re-syncs `\headwidth`. Keep top >= 1.1cm: the header hangs
  from the top margin, at 1.1cm its text stays ~4mm from the paper edge, and
  below 27pt the header box itself leaves the paper.
- `\kactlbinding{len}` switches to `twoside` with a binding offset. The cover
  is logical page 0, so LaTeX's odd/even parity is the reverse of the physical
  sheet sides; the two side margins are therefore swapped at
  `\begin{document}`, after every `\geometry` call.

### Whole document

- The contents page keeps its running header only because of
  `\tocloftpagestyle{fancy}` — tocloft otherwise forces `plain` there. It has
  no "Contents" title (`\contentsname` is empty).
- The notebook is **ragged-right on purpose** (`\AtBeginDocument{\raggedright}`):
  the body is three ~180pt columns, about 35 characters of prose, and
  justifying those opens gaps that read as rivers. It used to happen by
  accident, via `\@makechapterhead`'s `\raggedright` leaking out of an
  unbraced group. Side effect the content relies on: under `\raggedright`,
  `\\` ends the paragraph (`\@centercr`), so a paragraph ending in `\\` costs
  nothing.
- **Gone:** the per-chapter local table of contents (titletoc,
  `\startcontents`/`\printcontents`) and the appendix machinery.
  `content/appendix/` still exists but no `\kactlchapter` includes it. A
  `build/kactl.aux` left over from before the removal still calls
  `\ttl@writefile` and kills the next run with `! Undefined control
  sequence` — `make clean`.

### Column-filling invariants (do not break these)

Columns are ragged (`\raggedcolumns`) and the vertical list has *no*
stretchable glue: `parskip`, display skips, `\smallskipamount` and friends,
and the list skips (`\@listI`/`i`/`ii`/`iii` plus the `\@listi` that
`\small`/`\footnotesize` redefine) are all coerced to their natural width.
With no stretch every feasible break costs the same and TeX takes the last one
that fits, i.e. columns fill maximally; with stretch, a heading's
`\penalty-300` (or listings' -50, or our -100) beats "fill the column" and
opens third-of-a-column holes. `\@secpenalty` is zero and club/widow penalties
are 10000 for the same reason.

Things must also not come apart:

- `\interlinepenalty\@M` inside `\kactlheader` keeps a description atomic, and
  an lst `Init` hook does the same for wrapped code lines.
- listings' `\lst@Init` emits `\par\penalty-50` and `\lst@MakeCaption` starts
  with `\allowbreak`; both are `\patchcmd`'ed to `\nobreak` so a name line is
  never orphaned at a column bottom. The patches fail loudly
  (`\PackageError`) if listings changes under us.
- A caption keeps its first three code lines (`\lst@frameInit` patch plus the
  `Init`/`EveryLine` hooks), and a chapter head keeps its first template (the
  `\nobreak` at the end of `\@makechapterhead`, and `\kactlimport` emitting
  `\nobreak` instead of its own `\myneedspace`/`\penalty-100` when
  `\if@nobreak`, i.e. directly after a heading).
- `\myneedspace{3\baselineskip}` before a template reserves the name plus two
  lines. Raising it to 5 makes multicol open large gaps between templates,
  which is worse than an occasional stranded heading.

Diagnostics that worked: `pdftoppm -r 80 -f N -l N -png`, then vary one thing
per scratch copy (`make fast` is ~7 s). **Caveat:** the TOC is typeset inside
the multicols, so a scratch copy built in an empty `build/` (no `kactl.toc`)
has a different layout from the real PDF — copy `build/kactl.{aux,toc,out}`
first, or use `make kactl`. `\showbox` of each column inside
`\multi@column@out` shows which glue absorbed the slack (multicol sets
`\vbadness=10001`, so there are no Underfull warnings to grep for).

### Preprocessor rules

- The doc comment must contain `Author:` and `Description:`. Recognised
  commands are Author, Date, Description, Source, Time, Memory, License,
  Status, Usage, Details; only **Description, Usage, Time and Memory** are
  printed. Any other capitalised word followed by `:` at the start of a `* `
  line is an *unknown command* → **build error** (this bit us with "Bounds:").
  A `Word::` continuation line — C++ scope syntax, e.g. `LCT::link` — is not
  taken as a command.
- **Usage** is typeset as you write it: the preprocessor escapes it as code
  and wraps it in `\texttt` itself, so never add your own `\texttt{...}` (the
  braces are escaped and print literally). `_ { } ^ < >` are handled for you;
  hand-escape `& % # $` as `\&` etc. — an unescaped `& % #` is a fatal build
  error, `~`/`$` silently become a space / math — and write `\tilde` for `~`.
- **Description/Time/Memory** are LaTeX: only `<`/`>` are escaped for you,
  and `O(...)` in Time/Memory becomes `\bigo{...}`. Bare math like `\log` in a
  Description still needs `$...$`.
- `<hash>` markers work only in C++/Java listings, and only on lines that are
  actually printed: a marker on an `exclude-line`, or behind `///`, is a build
  error rather than a silently missing hash.
- Only *leading* indentation is tabified (each 4 spaces → one tab, also right
  after a leading `//` on commented-out code). Interior runs of spaces —
  alignment — are left alone.
- Every failure after option parsing is written into the output file as
  `\kactlerror{...}`, which stops pdflatex with the file name in the message.

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
