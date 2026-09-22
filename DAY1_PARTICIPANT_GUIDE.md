# PC3914 Day 1 Participant Execution Guide

This guide gives the exact command path for the Day 1 activities that use GDB and Valgrind/Memcheck. You should be able to follow these steps without relying on trainer-only command knowledge.

Use the Ubuntu/Linux environment that passed the PC3914 readiness check. Run all commands from the repository root unless the guide explicitly tells you to change directory.

## Start the correct Day 1 session

Each session uses a verified cumulative starter. From the repository root:

```sh
./tools/start-session day1-sessionN
cd work/day1-sessionN
make course-status
```

Replace `N` with the session number. If `work/day1-sessionN` already exists, do not overwrite it. Use your existing workspace or ask the trainer for the correct recovery path.

---

## Session 2: use Valgrind to verify the event store

### 1. Start Session 2

From the repository root:

```sh
./tools/start-session day1-session2
cd work/day1-session2
make course-status
```

### 2. Run the behavioural test first

```sh
make test-store
```

Expected successful evidence includes:

```text
test_event_store: OK
```

The behavioural test checks the event-store contract. It does not by itself prove that heap lifetime is clean.

### 3. Run the project Valgrind target

```sh
make valgrind-store
echo $?
```

The target runs the event-store test under Memcheck using the repository's strict error-exit rule. The underlying command is:

```sh
valgrind -q --leak-check=full --error-exitcode=9 ./build/test_event_store
```

A successful run exits with status `0`. Because the repository target uses `-q`, Valgrind may print little or no heap-summary text when the run is clean.

### 4. If you want to see the full Memcheck summary

After `make test-store` has built the test binary, run:

```sh
valgrind --leak-check=full ./build/test_event_store
```

For the corrected store, look for a clean result such as zero bytes in use at exit and no reported errors.

### What this proves

Use both forms of evidence:

- `make test-store`: the event-store behaviour is correct for the prepared tests.
- `make valgrind-store`: the same prepared path is clean under Memcheck's memory checks.

Neither one replaces the other.

---

## Session 3: diagnose the prepared crash with GDB

The Session 3 starter contains a deliberately broken diagnostic-snapshot fixture. The defect is isolated so the debugger has one clear cause to find.

### 1. Start Session 3

From the repository root:

```sh
./tools/start-session day1-session3
cd work/day1-session3
make course-status
```

Complete the arena and snapshot work for the session first. Before the GDB activity, these should pass:

```sh
make test-arena
make test-snapshot
```

### 2. Build the prepared broken binary

```sh
make gateway-broken
```

This builds:

```text
./build/gateway_broken
```

The prepared defect is in:

```text
src/broken/diag_snapshot_nocheck.c
```

### 3. Reproduce the crash outside GDB

```sh
./build/gateway_broken --arena 2048 data/burst_events.txt
echo $?
```

The broken build is expected to terminate with a segmentation fault. On a normal Linux shell, the exit status is commonly `139`.

The point of this step is only to establish that the failure is reproducible. The crash message does not yet explain the cause.

### 4. Start GDB with the same workload

```sh
gdb --args ./build/gateway_broken --arena 2048 data/burst_events.txt
```

At the `(gdb)` prompt:

```gdb
run
```

The program should stop at the crash.

### 5. Find the first relevant project frame

At the `(gdb)` prompt:

```gdb
bt
```

The innermost frame may be inside libc, for example `memcpy`. That is normal.

Find the first frame that belongs to the course source, normally in:

```text
src/broken/diag_snapshot_nocheck.c
```

Select that frame using the number shown by your own backtrace:

```gdb
frame N
list
```

Do not copy a frame number from another machine. Frame numbers can vary.

### 6. Inspect the state that proves the cause

First inspect the current function arguments and locals:

```gdb
info args
info locals
```

Then inspect the prepared failure evidence:

```gdb
print d
print (*arena).offset
print (*arena).capacity
```

If your source version exposes the requested copy length as a local, print that as well:

```gdb
print len
```

The important evidence is:

- the destination pointer `d` is `NULL`;
- the arena is near capacity;
- the requested copy cannot fit in the remaining arena space;
- the broken fixture continues into the copy instead of handling the legitimate allocation refusal.

### 7. State the diagnosis

Your answer should contain three things:

1. **What failed?** The program attempted to copy through a `NULL` destination.
2. **Where?** The failing copy is in `src/broken/diag_snapshot_nocheck.c`.
3. **Which state proves why?** The arena offset/capacity and requested size show that `arena_alloc` legitimately refused the request, but the caller failed to check the returned pointer.

Quit GDB with:

```gdb
quit
```

---

## Session 3: read four Memcheck signatures yourself

The same Session 3 starter contains a prepared Valgrind driver with one controlled mode at a time: leak, use-after-free, overflow, and clean.

### 1. Run the complete prepared demonstration

From `work/day1-session3`:

```sh
make valgrind-demo
```

The Make target builds `./build/valgrind_demo` and runs all four modes under Memcheck.

### 2. Run one mode at a time when you want to inspect it

Build the driver:

```sh
make build/valgrind_demo
```

Then run the individual modes:

```sh
valgrind -q --leak-check=full ./build/valgrind_demo leak
valgrind -q --leak-check=full ./build/valgrind_demo uaf
valgrind -q --leak-check=full ./build/valgrind_demo overflow
valgrind -q --leak-check=full ./build/valgrind_demo clean
```

### What to identify

**Leak**

Look for `definitely lost` and, where applicable, `indirectly lost`. These mean an owning reference was lost before the allocated memory was released.

**Use-after-free**

Look for an `Invalid read` together with the allocation/free history. The useful evidence connects the invalid access to the point where the object's lifetime ended.

**Overflow**

Look for an `Invalid write` immediately after an allocated block. In the prepared example, this represents a capacity/off-by-one error.

**Clean**

Use the clean mode as the contrast. A clean run should contain no invalid access report and no remaining in-use heap blocks.

The objective is not to memorize every Valgrind message. Learn to ask:

1. What class of memory error is reported?
2. Where did the invalid access occur?
3. Where was the affected block allocated or freed?
4. Which ownership or bounds rule was violated?

---

## Session 4: Valgrind can reveal a bug that a plain test misses

In `work/day1-session4`, Challenge A intentionally demonstrates that correct-looking values do not prove memory-safe traversal.

Run the plain test:

```sh
make challenge-a
```

Then run the memory evidence:

```sh
make challenge-a-valgrind
```

Before the repair, the plain run can remain green while Valgrind reports an invalid read just past the end of the allocation. After the repair, rerun:

```sh
make challenge-a
make challenge-a-valgrind
```

Both must succeed.

---

## First recovery checks

If a command in this guide does not behave as described, check these before changing source code.

### Wrong directory or wrong session

```sh
pwd
make course-status
```

Confirm you are inside the `work/day1-sessionN` directory required by the activity.

### GDB or Valgrind command not found

```sh
gdb --version
valgrind --version
```

If either command is missing, return to `docs/SETUP.md` and the readiness check. Do not substitute a different debugger or memory checker during the governed practical.

### GDB cannot debug the process

If GDB reports a ptrace/permission restriction, stop and show the exact error to the trainer. This is an environment problem, not a reason to rewrite the exercise.

### A prepared Make target does not exist

Run:

```sh
make course-status
pwd
```

Then confirm that you started the correct session with `./tools/start-session`. Do not create an ad-hoc replacement target during class.

---

## Day 1 tool checkpoint

By the end of Day 1 you should be able to do all of the following without a hidden trainer command:

- run a normal test and explain what property it proves;
- run a program under Valgrind/Memcheck and identify leak, invalid-read/write and clean signatures;
- launch a prepared failing program under GDB;
- use `run`, `bt`, `frame`, `list`, `info args`, `info locals` and `print`;
- form a diagnosis from runtime state rather than from the crash message alone;
- rerun the relevant test/tool after a repair and state the evidence that now passes.
