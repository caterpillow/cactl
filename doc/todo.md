# Notebook to-do (from the October 2026 review)

Owner's notes recorded during the review of the first 140 findings; the review
artifact is https://claude.ai/artifact/Krj8Eh2HF6Jjx5TMPEEhiJ.

## Deferred ("later")
- `.vimrc` Alt-key mappings (F002): they work in the expected environment;
  confirm on the contest machines (already on the test-session list).
- Stokes' theorem orientation convention in the math chapter (F026).
- Markov chains: `pi_i = 1/E(T_i)` needs a finite chain; degree-proportional
  stationary distribution vs the non-bipartite condition (F033, F034). Owner
  does not yet know what these mean; learn before touching.
- `MonotonicCHT.h` min usage (slopes must be non-increasing after negation,
  F087): owner is thinking of redoing the template.
- `FastSubsetTransform.h`: AND/OR transforms are complement-indexed, so the
  transform is only usable through `conv` (F136). Learn what this means;
  until then only use `conv`.
- Series / generating-function material for the math chapter (F029): owner
  will do the GF material later.

## Refactors the owner plans
- `TreapBeats.h`: status set to "review" after the int-overflow fixes
  (F112-F114); re-verify before relying on it.
- `BerlekampMassey.h`: owner wants the recurrence/initial-conditions
  convention spelled out (F123); done in the Description, re-read it.

## Review status
- Findings F001-F140 decided on 2026-10-06; the rest were re-curated
  (duplicates folded, fixes shortened) and remain to be reviewed in the
  artifact.

## Added on 2026-10-07 (second review sitting)
- Deferred additions: M011 huge exponents (already partly in phiFunction.h), M012 prime
  powers in factorials (Legendre/Kummer), M019 Fibonacci identities and Pisano periods,
  M022 triangle centres / exradii / Stewart.
- T010 k-th smallest in a range: owner wants it as an addition to the existing
  PersistentSegtree.h rather than a new file; a sketch was proposed in chat.
