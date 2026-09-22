# PC3914 Day 3 Delivery Validation

Branch scope: Day 3 participant-delivery validation only. Locked v2.17 session archives were not modified.

## Starter identity

The Day 3 archives in the locked v2.17 source and repository manifest are:

- Session 1: `66583250e172f6c6545d6d9c8fe65c258a59b17f81459f890b5765b5eee734ba`
- Session 2: `96a126c74e52403290d5c656e6eaeb9d33dc816d0a2050449c46846d063a817e`
- Session 3: `35655854d34323a045f27a90d3b5cb38c75ccab96ea06fe794b45538451fe2b9`
- Session 4: `dfd2c94eee1250dbcd18ecee786a922d74f81972c7b1b2bc5a4ea35006b824a5`

Fresh local SHA-256 verification matched all four.

## Untouched starter-stop evidence

All four fresh-extracted entry snapshots build successfully with GCC/C11 and `-Wall -Wextra -Werror`.

- Session 1: `make test-dir` stops at `TODO-D3.1a`.
- Session 2: `make test-work-queue` stops at `TODO-D3.3`.
- Session 3: `make test-framing` stops at `TODO-D3.5`.
- Session 4: `make test-observer` stops at `TODO-D3.7`.

These are intentional participant task boundaries.

## Disposable solved-path validation

No solved implementation was written to Git.

For each session, a disposable clean starter received only that session's participant-edit source files from the locked trainer final reference, then the documented participant verification targets were executed.

### Session 1

Passed:

- `make test-dir`
- `make run-dir` — 19 events, 0 rejected
- missing-directory CLI path — clean non-zero failure
- `make test-pipe`
- `make run-report`
- `make day3-block1-check`

Observed:

- short-write and EINTR cases handled;
- child EOF/reap path passed;
- reporter exited status 0;
- heap reported 0 live blocks;
- `DAY 3 BLOCK 1 CHECK: PASS`.

### Session 2

Passed:

- `make test-work-queue`
- `make test-workers`
- `make run-workers`
- `make day3-block2-check`

Observed:

- 3 producers x 200 items / 3 consumers: 600 items exactly once;
- worker drain/join and idle-stop groups passed;
- `--workers 1` report matched the single-threaded run;
- `DAY 3 BLOCK 2 CHECK: PASS`.

### Session 3

Passed:

- `make test-framing` — all seven groups including `[eof-tail]`;
- `make test-reactor` — all four groups including `[hard-error]`;
- `make cli-regression`;
- `make net-check` — two reports byte-identical;
- `make day3-block3-check`.

Observed final line:

`DAY 3 BLOCK 3 CHECK: PASS`.

### Session 4

Passed:

- `make test-observer`;
- `make test-diag-binary` — all seven groups;
- `make block4-cli-regression`;
- `make day3-final-check`.

Observer evidence included metrics `{5, 2, 1}`, ordered subscriber execution, and 0 live allocations.

Final evidence:

`DAY 3 FINAL CHECK: PASS - Valgrind optional where unavailable`.

## Tool boundary

The governed Day 3 source explicitly treats Valgrind as optional where unavailable and does not claim a fresh Valgrind PASS from the source QA runtime. The Makefile cumulative gates used here likewise report Valgrind as optional where unavailable.

ThreadSanitizer is an optional Session 2 command (`make tsan-queue`) and is not required for the participant completion path documented in `DAY3_PARTICIPANT_GUIDE.md`.
