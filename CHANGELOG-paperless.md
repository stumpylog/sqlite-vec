# paperless integration line changelog

Changes carried by the `paperless` branch of `stumpylog/sqlite-vec` beyond upstream `asg017/sqlite-vec`. Each release is an annotated tag `v0.1.10-paperless.N`, built into the wheel `0.1.10+paperless.N` by `paperless-ngx/builder`. Drop an entry from the carried set when upstream merges it.

## v0.1.10-paperless.2 (unreleased)

Performance for filtered KNN (one-document chat queries no longer read the whole index), plus delete and scan improvements.

Merged from upstream, as separate merge commits:

- [#330](https://github.com/asg017/sqlite-vec/pull/330) Skip reading a chunk's vectors when the KNN filters keep none of its rows (the key fix for paperless-ngx#14600)
- [#327](https://github.com/asg017/sqlite-vec/pull/327) Fix `rowid IN (...)` on KNN queries when rowids are far apart
- [#328](https://github.com/asg017/sqlite-vec/pull/328) Binary-search an integer metadata column's `IN (...)` list
- [#329](https://github.com/asg017/sqlite-vec/pull/329) Make KNN filters keep the rows SQLite keeps
- [#364](https://github.com/asg017/sqlite-vec/pull/364) AVX cosine distance for float vectors
- [#337](https://github.com/asg017/sqlite-vec/pull/337) Select each chunk's k nearest rows with the `min_idx()` heap
- [#358](https://github.com/asg017/sqlite-vec/pull/358) Check only the deleted row's validity bit on DELETE
- [#359](https://github.com/asg017/sqlite-vec/pull/359) Check a DELETE's chunk for emptiness through its validity handle
- [#336](https://github.com/asg017/sqlite-vec/pull/336) Read a full scan's vectors through one blob handle per chunk (test-file conflict with the parser-guard test resolved by keeping both)

Fork-only:

- Runtime-dispatch #364's AVX cosine kernel: `target("avx,avx2")` on the kernel and `__builtin_cpu_supports("avx2")` at the call site. As merged it was unguarded, which is the pre-AVX SIGILL class fixed by #307.

Considered and not carried: [#338](https://github.com/asg017/sqlite-vec/pull/338) (cosine sums in double, nobody has hit it), [#316](https://github.com/asg017/sqlite-vec/pull/316) (conflicts with #309 and #329), [#339](https://github.com/asg017/sqlite-vec/pull/339) (NEON, Makefile conflict), [#243](https://github.com/asg017/sqlite-vec/pull/243), [#269](https://github.com/asg017/sqlite-vec/pull/269).

## v0.1.10-paperless.1 plus two commits on `paperless` (included in .2)

- [#319](https://github.com/asg017/sqlite-vec/pull/319) vec0 parser token-type guards: `&&` to `||` (20 sites)

## v0.1.10-paperless.1

Base: upstream `v0.1.10-alpha.4`.

- [#303](https://github.com/asg017/sqlite-vec/pull/303) Locale-dependent JSON float parsing (fixes #241)
- [#304](https://github.com/asg017/sqlite-vec/pull/304) Link ordering, link libs scoped per target
- [#305](https://github.com/asg017/sqlite-vec/pull/305) `test-unit.c` compiles and links, plus `test-unit` passing
- [#306](https://github.com/asg017/sqlite-vec/pull/306) Vendored SQLite amalgamation 3.53.2
- [#307](https://github.com/asg017/sqlite-vec/pull/307) AVX/AVX2 dispatch: target attributes and runtime CPU detection (upstream issue #302)
- [#309](https://github.com/asg017/sqlite-vec/pull/309) Distance constraints in rescore KNN queries (fixes #308)
- [#310](https://github.com/asg017/sqlite-vec/pull/310) Reject malformed vector column types
- [#311](https://github.com/asg017/sqlite-vec/pull/311) Clamp DiskANN int8 quantization
- [#318](https://github.com/asg017/sqlite-vec/pull/318) OOB read on the metadata `IN` path

Fork-only: Makefile `EXTRA_CFLAGS` hook; CI gating for `paperless`, PRs and tags; ASan/UBSan; nightly valgrind and CodeQL.
