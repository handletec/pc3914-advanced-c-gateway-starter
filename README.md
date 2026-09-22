# PC3914 Advanced C Gateway — Day 1

Participant repository for **PC3914 — Advanced C Programming: From Basic C to Systems-Ready**, material version **v2.17**.

For the current delivery, this repository publishes **Day 1 only**. Day 2 and Day 3 will be added separately.

## Existing participants

From your existing clone:

```sh
git status
git pull
```

If `git pull` reports that your local tracked changes would be overwritten, do **not** force, reset, stash, or merge during class. Keep that folder as your old work and make a fresh clone instead.

## Start Day 1 Session 1

From the repository root:

```sh
./tools/start-session day1-session1
cd work/day1-session1
make course-status
```

Do all exercise work inside `work/day1-session1/`.

## Later Day 1 sessions

Return to the repository root and start the required cumulative snapshot:

```sh
./tools/start-session day1-session2
./tools/start-session day1-session3
./tools/start-session day1-session4
```

Then enter the corresponding directory:

```sh
cd work/day1-sessionN
make course-status
```

Each later session snapshot already contains the completed prerequisite state expected at that point in the course. Your earlier `work/` folders are left untouched.

## Important

- Work only inside `work/<session-id>/`.
- Do not edit `session-starters/`.
- `start-session` verifies the governed SHA-256 of the selected starter before extracting it.
- `start-session` refuses to overwrite an existing work directory.
- The original initial starter files remain at repository root for compatibility; use `tools/start-session` for classroom progression.

## Day 1 session map

- `day1-session1` — bounded parsing and parser contracts
- `day1-session2` — retained data, ownership and Event Store
- `day1-session3` — arena allocation, snapshot lifetime and integration evidence
- `day1-session4` — regression challenges and cumulative Day 1 evidence
