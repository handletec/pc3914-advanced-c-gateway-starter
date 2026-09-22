# PC3914 Advanced C Gateway

Participant repository for **PC3914 — Advanced C Programming: From Basic C to Systems-Ready**, material version **v2.17**.

This repository is the classroom entry point for all **12 sessions across Days 1–3**. Clone once, then use the session helper to create a separate verified workspace for the session you are starting.

## First-time setup

Use the governed Ubuntu/Linux environment described in `docs/SETUP.md`, then run:

```sh
./tools/pc3914_readiness_check_v2.17.sh
```

Required final line:

```text
PC3914 READINESS: PASS
```

## Start a session

From the repository root:

```sh
./tools/start-session day1-session1
cd work/day1-session1
make course-status
```

For the next session, return to the repository root and run the helper with the next session ID:

```sh
./tools/start-session day1-session2
cd work/day1-session2
make course-status
```

The same workflow continues through `day3-session4`. See `SESSION_STARTERS.md` for the complete session map.

## Participant execution guides

Use these when you need the exact command path rather than relying on trainer memory:

- **Day 1:** [`DAY1_PARTICIPANT_GUIDE.md`](DAY1_PARTICIPANT_GUIDE.md) — exact GDB and Valgrind/Memcheck workflows, including the prepared `gateway_broken` crash.
- **Day 2:** [`DAY2_PARTICIPANT_GUIDE.md`](DAY2_PARTICIPANT_GUIDE.md) — step-by-step session execution path.
- **Day 3:** [`DAY3_PARTICIPANT_GUIDE.md`](DAY3_PARTICIPANT_GUIDE.md) — files/processes, threads, TCP/Reactor, Observer/persistence, and final integration.

## How the session workspaces behave

- Each later session starter is a **cumulative verified entry state**: prerequisite course functionality is already completed up to that point.
- Your earlier `work/<session-id>/` folders are left untouched.
- Work from an earlier folder is not automatically merged into a later starter; each session is a separate course checkpoint.
- `tools/start-session` verifies the governed SHA-256 before extracting anything.
- The helper detects the actual project root in the archive instead of assuming every day has the same archive layout.
- The helper refuses to overwrite an existing `work/<session-id>/`.
- Do not edit files in `session-starters/`.

The initial source files kept at repository root are retained for compatibility. For classroom progression, the canonical workflow is always `tools/start-session`.

## Existing participants

Before updating an existing clone:

```sh
git status
git pull
```

If `git pull` reports that tracked local changes would be overwritten, do **not** force, reset, stash, or merge during class. Keep that folder as your previous work and make a fresh clone instead.

After updating, use `tools/start-session` and do new exercise work only inside `work/<session-id>/`.

## Session IDs

```text
day1-session1  day1-session2  day1-session3  day1-session4
day2-session1  day2-session2  day2-session3  day2-session4
day3-session1  day3-session2  day3-session3  day3-session4
```

For the exact archive names and session topics, see `SESSION_STARTERS.md`.
