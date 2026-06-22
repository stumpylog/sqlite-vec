# Deferred fixes / follow-ups (sub-project A)

Tracking items deliberately deferred during the fork integration line + CI work
(plan: `plans/2026-06-19-fork-integration-line-and-ci.md`). None block the
`v0.1.10-paperless.1` tag.

## 1. vec0 parser `&&` -> `||` token guards (uninitialised reads) — DEFERRED

**What:** All 20 "next token must be X" guards in the vec0 column-definition
parser use `&&` where they need `||`:

```c
rc = vec0_scanner_next(&scanner, &token);
if (rc != VEC0_TOKEN_RESULT_SOME &&        // BUG: should be ||
    token.token_type != TOKEN_TYPE_IDENTIFIER) {
  return SQLITE_EMPTY;
}
```

When the scanner returns no token, `token` is uninitialised; with `&&` the code
only bails if the *uninitialised* `token.token_type` also happens to mismatch,
otherwise it falls through and dereferences uninitialised `token.start`/`.end`.
It also under-rejects: a present-but-wrong token type is accepted.

**Sites:** `sqlite-vec.c`, functions `vec0_parse_vector_column`,
`vec0_parse_partition_key_definition`, and the auxiliary/metadata parse helpers.
20 occurrences of `rc != VEC0_TOKEN_RESULT_SOME &&` (all pre-existing in the
upstream base `04d28bd`).

**How found:** the Tier-2 nightly valgrind memcheck job (`analyze.yaml`,
`valgrind-unit`) on `dist/test-unit` reports ~21 "Conditional jump / Use of
uninitialised value" errors in `vec0_parse_*` and `sqlite3_strnicmp` called from
them. ASan/UBSan (Tier-1, per-PR) does NOT catch these — uninitialised-read is
valgrind/MSan territory — so the per-PR gate is green and the tag is unaffected.

**Fix:** flip all 20 guards `&&` -> `||`. Behavior becomes correctly stricter
(malformed column definitions now error instead of falling through), so
re-run `make test-loadable` and review any snapshot deltas before committing.
Do it on a topic branch off the upstream base so it doubles as an
`asg017/sqlite-vec` PR; then cherry-pick onto `paperless`. Until then the
nightly valgrind job will report these known findings.

## 2. Fuzz harness build is broken (per-PR smoke deferred) — DEFERRED

**What:** `make -C tests/fuzz all` fails before compiling anything:
`tests/fuzz/Makefile:158: *** missing separator`. The fuzz target list uses
backslash line-continuations, but the `ivf_*` group line (ending
`ivf_knn_deep ivf_cell_overflow ivf_rescore`) is missing its trailing `\`, so
make treats the following `diskann_*` line as a malformed statement. Pre-existing
upstream (identical to base `04d28bd`); `fuzz-linux` has always been red there,
and `fuzz-macos`/`fuzz-windows` are already `continue-on-error` best-effort.

**Impact here:** Task 8 originally added a per-PR fuzz smoke (a `pull_request` +
`push: paperless` trigger on `fuzz.yaml`). Because the harness does not build,
that made the gate red, so the trigger was reverted — `fuzz.yaml` now matches
upstream (runs only on push-to-main, which never happens on this fork, plus the
nightly schedule and manual dispatch). No fuzz coverage gates the integration
line until the harness is repaired.

**Fix (separate, larger task):** add the missing `\` at the end of the
`ivf_*` target-list line, then actually validate the harness builds and runs
against the current base (SQLite 3.53.2, #307 SIMD changes, etc.) — the
separator is only the first error make reaches. Once green, re-add the per-PR
fuzz smoke (short `-max_total_time`, the 60s default) and wire it to `paperless`
+ `pull_request`. Upstreamable.

## 3. vlasky `optimize` space-reclaim (#210) — DEFERRED

Skipped for `paperless.1`. vlasky's port predates the v0.1.10 command-column
mechanism (it reinvents a parallel `TABLE_NAME`/`SpecialInsert` command path)
and bundles an unrelated `rowid PRIMARY KEY` -> `INTEGER PRIMARY KEY` shadow
schema change the base deliberately rejected. Nothing consumes `optimize` yet
(the paperless `vector_store.py` refactor is sub-project C, out of scope) and it
has high data-corruption blast radius. Re-add later as a dedicated clean port
wired into the existing command dispatch chain (alongside rescore/ivf/diskann),
with attribution to Vlad Lasky and the original PR author.
