# paperless integration line changelog

Changes carried by the `paperless` branch of `stumpylog/sqlite-vec` beyond upstream `asg017/sqlite-vec`. Each release is an annotated tag `v0.1.10-paperless.N`, built into the wheel `0.1.10+paperless.N` by `paperless-ngx/builder`. Drop an entry from the carried set when upstream merges it.

Each upstream PR is carried as one squash commit. Fixes or follow-ups to it are separate commits listed beneath it. Entries read: upstream PR, fork commit, summary.

## v0.1.10-paperless.2

Performance for filtered KNN (one-document chat queries no longer read the whole index), plus delete and scan improvements.

- [#319](https://github.com/asg017/sqlite-vec/pull/319) [`c37b3e3`](https://github.com/stumpylog/sqlite-vec/commit/c37b3e3) vec0 parser token-type guards: `&&` to `||` (20 sites)
- [#330](https://github.com/asg017/sqlite-vec/pull/330) [`ef56ce5`](https://github.com/stumpylog/sqlite-vec/commit/ef56ce5) Skip reading a chunk's vectors when the KNN filters keep none of its rows (the key fix for paperless-ngx#14600)
- [#327](https://github.com/asg017/sqlite-vec/pull/327) [`2f74553`](https://github.com/stumpylog/sqlite-vec/commit/2f74553) Fix `rowid IN (...)` on KNN queries when rowids are far apart
- [#328](https://github.com/asg017/sqlite-vec/pull/328) [`77cddd4`](https://github.com/stumpylog/sqlite-vec/commit/77cddd4) Binary-search an integer metadata column's `IN (...)` list
- [#329](https://github.com/asg017/sqlite-vec/pull/329) [`cda6bca`](https://github.com/stumpylog/sqlite-vec/commit/cda6bca) Make KNN filters keep the rows SQLite keeps
- [#364](https://github.com/asg017/sqlite-vec/pull/364) [`509e456`](https://github.com/stumpylog/sqlite-vec/commit/509e456) AVX cosine distance for float vectors
  - [`4d23d6d`](https://github.com/stumpylog/sqlite-vec/commit/4d23d6d) Fork follow-up: runtime-dispatch the AVX kernel with `target("avx,avx2")` and `__builtin_cpu_supports("avx2")`. As merged it was unguarded, which is the pre-AVX SIGILL class fixed by #307.
- [#337](https://github.com/asg017/sqlite-vec/pull/337) [`891c93c`](https://github.com/stumpylog/sqlite-vec/commit/891c93c) Select each chunk's k nearest rows with the `min_idx()` heap
- [#358](https://github.com/asg017/sqlite-vec/pull/358) [`d53b897`](https://github.com/stumpylog/sqlite-vec/commit/d53b897) Check only the deleted row's validity bit on DELETE
- [#359](https://github.com/asg017/sqlite-vec/pull/359) [`b548f71`](https://github.com/stumpylog/sqlite-vec/commit/b548f71) Check a DELETE's chunk for emptiness through its validity handle
- [#336](https://github.com/asg017/sqlite-vec/pull/336) [`5357cbc`](https://github.com/stumpylog/sqlite-vec/commit/5357cbc) Read a full scan's vectors through one blob handle per chunk (test-file conflict with the parser-guard test resolved by keeping both)

Fork-only:

- [`c60194e`](https://github.com/stumpylog/sqlite-vec/commit/c60194e) Tests: filtered KNN with `rowid IN` plus distance, and after delete and reinsert
- [`aa1077d`](https://github.com/stumpylog/sqlite-vec/commit/aa1077d), [`7859399`](https://github.com/stumpylog/sqlite-vec/commit/7859399) CI: drop the Windows, Android, iOS, wasm, pyodide, ncruces and cosmopolitan jobs; run the macOS arm64 job on `macos-latest`

Considered and not carried: [#338](https://github.com/asg017/sqlite-vec/pull/338) (cosine sums in double, nobody has hit it), [#316](https://github.com/asg017/sqlite-vec/pull/316) (conflicts with #309 and #329), [#339](https://github.com/asg017/sqlite-vec/pull/339) (NEON, Makefile conflict), [#243](https://github.com/asg017/sqlite-vec/pull/243), [#269](https://github.com/asg017/sqlite-vec/pull/269).

## v0.1.10-paperless.1

Base: upstream `v0.1.10-alpha.4`.

- [#303](https://github.com/asg017/sqlite-vec/pull/303) [`72e8dc0`](https://github.com/stumpylog/sqlite-vec/commit/72e8dc0) Locale-dependent JSON float parsing (fixes #241)
- [#304](https://github.com/asg017/sqlite-vec/pull/304) [`7bba778`](https://github.com/stumpylog/sqlite-vec/commit/7bba778) Link ordering, link libs scoped per target
- [#305](https://github.com/asg017/sqlite-vec/pull/305) [`0fc868f`](https://github.com/stumpylog/sqlite-vec/commit/0fc868f), [`38d0374`](https://github.com/stumpylog/sqlite-vec/commit/38d0374) `test-unit.c` compiles and links, plus `test-unit` passing
- [#306](https://github.com/asg017/sqlite-vec/pull/306) [`fc883b8`](https://github.com/stumpylog/sqlite-vec/commit/fc883b8) Vendored SQLite amalgamation 3.53.2
- [#307](https://github.com/asg017/sqlite-vec/pull/307) [`e9dc712`](https://github.com/stumpylog/sqlite-vec/commit/e9dc712) AVX/AVX2 dispatch: target attributes and runtime CPU detection (upstream issue #302)
- [#309](https://github.com/asg017/sqlite-vec/pull/309) [`2b693ee`](https://github.com/stumpylog/sqlite-vec/commit/2b693ee) Distance constraints in rescore KNN queries (fixes #308)
- [#310](https://github.com/asg017/sqlite-vec/pull/310) [`d96adef`](https://github.com/stumpylog/sqlite-vec/commit/d96adef) Reject malformed vector column types
- [#311](https://github.com/asg017/sqlite-vec/pull/311) [`8193e23`](https://github.com/stumpylog/sqlite-vec/commit/8193e23) Clamp DiskANN int8 quantization
- [#318](https://github.com/asg017/sqlite-vec/pull/318) [`e5a849f`](https://github.com/stumpylog/sqlite-vec/commit/e5a849f) OOB read on the metadata `IN` path

Fork-only: Makefile `EXTRA_CFLAGS` hook ([`0bd1856`](https://github.com/stumpylog/sqlite-vec/commit/0bd1856)); CI gating for `paperless`, PRs and tags ([`e68e4e7`](https://github.com/stumpylog/sqlite-vec/commit/e68e4e7)); ASan/UBSan ([`e238357`](https://github.com/stumpylog/sqlite-vec/commit/e238357)); nightly valgrind and CodeQL ([`b0f1df6`](https://github.com/stumpylog/sqlite-vec/commit/b0f1df6)).
