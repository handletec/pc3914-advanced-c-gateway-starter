# PC3914 Day 2 Delivery Validation

Branch scope: Day 2 participant-delivery validation only. Locked v2.17 session archives were not modified.

## Starter identity

The Day 2 archives in `session-starters/SHA256SUMS` remain:

- Session 1: `1a054878b273456814c424d851927756a78246009a99d567078059598f30d891`
- Session 2: `8912a0220c6e12ddd0d9b443ca0fd87d0a9d4af6379d725c913df1cf8762aeac`
- Session 3: `1e21b73558560580ee2fcad264f1ec25276bc6c197c75279d52ece3527c596ba`
- Session 4: `0e7f9996f92f6a1c8bb0018b09aed02bf91875cd8b09fb857df1fc5fd1e8e37d`

## Untouched starter-stop evidence

All four clean entry snapshots build successfully with GCC/C11 and `-Wall -Wextra -Werror`.

- Session 1: `make test-day2` stops at the prepared TODO-D2.1 tail invariant.
- Session 2: `make test-day2-block2` stops at the prepared TODO-D2.4 collision invariant.
- Session 3: `make opaque-check` reports that private registry access still compiles.
- Session 4: `make test-day2-block4` stops at the prepared TODO-D2.7 conservative-policy assertion.

These are intentional participant task boundaries.

## Disposable solved-path validation

No solved code was written to Git.

A disposable local copy was used to validate the documented completion path:

- Session 1: queue, sink and stack suites pass; burst input reports 400 queued / 400 processed / 0 still queued; 0 live blocks.
- Session 2: registry basic/collision/growth, heap order/ties/growth and BST suites pass; burst input tracks 4 devices, raises 154 alerts and exits with 0 live blocks.
- Session 3: opaque check passes for the intended incomplete-type reason; unchanged registry tests pass; static/shared core builds and consumers run.
- Session 3 manual lab: missing archive produces the intended undefined-reference link failure; unconfigured shared client exits 127 with loader discovery failure; `LD_LIBRARY_PATH=build/lab` repairs it; `make lab-check` reports `LINK LAB: PASS`.
- Session 4: Strategy and State suites pass; state matrix reports 8 transitions / 8 no-ops / 4 rejections; policy comparison reports 154 default vs 180 conservative alerts; trace/no-trace functional output matches.

## Tool limitation

Valgrind is not installed in the current validation runtime. No fresh Valgrind PASS is claimed here. The locked course environment/readiness contract remains responsible for Valgrind-dependent final gates.
