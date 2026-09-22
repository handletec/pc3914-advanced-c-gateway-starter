# PC3914 Day 3 Participant Activity Guide

This guide is the classroom navigation for **Day 3** of PC3914 v2.17.

The session starter archives are governed and must not be edited. Use the repository helper to create a separate workspace for each session, then do all exercise work inside that workspace.

## Before Day 3

From the repository root:

```sh
git status
git pull
```

If `git pull` refuses because tracked local changes would be overwritten, keep that folder as your previous work and make a fresh clone. Do not force, reset, stash, or merge during class.

For each Day 3 session:

```sh
./tools/start-session <session-id>
cd work/<session-id>
make course-status
```

The untouched starter is expected to stop at the current exercise boundary. That failure is the starting evidence, not a broken package.

Before a substantial edit you may save the current session source with:

```sh
make checkpoint
```

---

## Day 3 Session 1 — Files, Processes and Pipes

### Start the session

From the repository root:

```sh
./tools/start-session day3-session1
cd work/day3-session1
make course-status
make test-dir
```

Expected untouched result: the directory test stops at `TODO-D3.1a` because `alpha.txt` is not yet accepted as an event input.

### Activity — Filter Event Files and Build Paths Without Truncation

**File:** `src/event_dir.c`  
**Functions:** `event_dir_accept`, `event_dir_join`  
**TODOs:** `TODO-D3.1a`, `TODO-D3.1b`

Implement only the filename filter and bounded path join.

Required rules:

- reject dot entries;
- accept only names whose **final suffix** is `.txt`;
- build `<dir>/<name>` with bounded `snprintf`;
- if the would-have-written length is outside capacity, clear the destination when possible and return `GW_ENOSPACE`.

Do not change the prepared `opendir` / `readdir` / `stat` / sort / `closedir` logic, tests, or fixtures.

Verify:

```sh
make test-dir
make run-dir
```

The directory run should process the intended fixture set and report:

```text
parser: 19 events, 0 rejected
```

Also exercise the missing-directory CLI error path:

```sh
./gateway --events-dir /no/such/pc3914-dir
```

This command is expected to fail cleanly with a non-zero exit status and a useful error; do not change the code to make a missing directory succeed.

### Lab — Complete the Pipe I/O and Descriptor-Ownership Protocol

Work in:

- `src/gw_io.c::write_all` — `TODO-D3.2a`
- `src/health_pipe.c` child setup — `TODO-D3.2b`
- `src/health_pipe.c` parent setup — `TODO-D3.2c`

Required behaviour:

- `write_all` continues until every byte is written;
- retry `write` on `EINTR`;
- zero-byte progress with bytes remaining is `GW_EIO`;
- child closes its write end, duplicates the read end onto stdin, then closes the original read descriptor before `exec`;
- parent closes its read end immediately after `fork`.

Do not change the prepared `finish_reporter` close/reap sequencing, tests, or reporter implementation.

Verify:

```sh
make test-pipe
make run-report
make day3-block1-check
```

Completion evidence includes:

```text
test_health_pipe: all groups passed
health_reporter: ... exited with status 0
heap: ... 0 live at exit
DAY 3 BLOCK 1 CHECK: PASS
```

---

## Day 3 Session 2 — Threads, Thread-Safe Work Queue and Worker Shutdown

### Start the session

From the repository root:

```sh
./tools/start-session day3-session2
cd work/day3-session2
make course-status
make test-work-queue
```

Expected untouched result: the queue test stops at `TODO-D3.3` because push on a closed queue does not yet return `GW_ECLOSED`.

### Activity — Make `work_queue_push` Safe for Many Producers

**File:** `src/work_queue.c`  
**Function:** `work_queue_push`  
**TODO:** `TODO-D3.3`

Protect the queue invariant with the existing mutex.

Required behaviour:

- reject a closed queue with `GW_ECLOSED`;
- on rejection or allocation failure, caller ownership of the event remains unchanged;
- enqueue through the existing Day 2 FIFO;
- signal one waiter only after a successful push;
- unlock on every return path.

Do not edit `work_queue_pop`, `work_queue_close`, `worker_pool_stop`, or the tests yet.

Verify:

```sh
make test-work-queue
```

After this activity the producer/ownership checks should pass and the suite should advance to the next intended TODO.

### Lab — Complete Pop, Close and Worker Shutdown

Edit only:

- `src/work_queue.c::work_queue_pop` — `TODO-D3.4a`
- `src/work_queue.c::work_queue_close` — `TODO-D3.4b`
- `src/worker_pool.c::worker_pool_stop` — `TODO-D3.4c`

Required behaviour:

- wait in a `while` loop while the queue is empty and open;
- pop under the queue lock, then release the lock before application work;
- close under the mutex and broadcast to all waiters;
- repeated close is harmless;
- stop closes the queue, joins workers, then publishes totals;
- if any join fails, return `GW_EIO` and do not publish a plausible `processed_out` total.

Do not modify the prepared `worker_main`, `worker_pool_start`, gateway state-processing code, or tests.

Verify:

```sh
make test-work-queue
make test-workers
make run-workers
make day3-block2-check
```

Required evidence includes:

```text
test_work_queue: all groups passed
test_worker_pool: all groups passed
--workers 1: report identical to the single-threaded run
DAY 3 BLOCK 2 CHECK: PASS
```

Optional sanitizer evidence in a supported environment:

```sh
make tsan-queue
```

---

## Day 3 Session 3 — TCP Clients, Stream Framing and One `poll()` Reactor

### Start the session

From the repository root:

```sh
./tools/start-session day3-session3
cd work/day3-session3
make course-status
make test-framing
```

Expected untouched result: the framing suite stops at `TODO-D3.5`; two fragments do not yet become one complete event.

### Activity — Frame Arbitrarily Fragmented TCP Input

**File:** `src/stream_framer.c`  
**Function:** `client_feed`  
**TODO:** `TODO-D3.5`

Implement bounded stream framing over arbitrary TCP chunk boundaries.

Required behaviour:

- append only bytes that fit;
- parse complete newline-terminated records as they arrive;
- compact consumed data;
- retain an incomplete tail at the front of the buffer;
- if the buffer becomes full without a consumable complete record, return `GW_ENOSPACE`;
- clone owned events before the borrowed receive-buffer lifetime ends.

Do not change the prepared parse/compact helper, prepared `client_read`, or tests.

Verify:

```sh
make test-framing
```

Required result is seven passing groups, including:

```text
[eof-tail]   OK
[half+half]  OK
[two-in-one] OK
[tail]       OK
[clone]      OK
[bounds]     OK
[socketpair] OK
test_stream_framing: all groups passed
```

### Lab — Complete the `poll()` Reactor Handlers

**File:** `src/reactor.c`  
**Functions:** `handle_listener`, `handle_client`  
**TODOs:** `TODO-D3.6a`, `TODO-D3.6b`

Listener rules:

- accept repeatedly until the socket would block;
- retry `EINTR`;
- propagate hard accept errors;
- set accepted descriptors non-blocking;
- if all fixed slots are occupied, accept and immediately close the extra client so the listener does not remain permanently readable;
- initialize one fixed free slot on success.

Client rules:

- call the prepared read/framing path;
- keep a live would-block client;
- drop an orderly close once;
- drop a hard-error client once.

Do not change the prepared `reactor_step` / `poll` loop, slot-drop bookkeeping, tests, or CLI parser outside TODO regions.

Verify:

```sh
make test-reactor
make cli-regression
make net-check
make day3-block3-check
```

Required evidence includes:

```text
test_reactor: all groups passed
CLI REGRESSION: PASS
NET CHECK: reports byte-identical across runs
DAY 3 BLOCK 3 CHECK: PASS
```

The Reactor suite must include the `[hard-error]` group. Network mode must continue to reject a stateful multi-worker configuration such as `--listen ... --workers 3`.

---

## Day 3 Session 4 — Observer Pattern, Persistence and Final Integration

### Start the session

From the repository root:

```sh
./tools/start-session day3-session4
cd work/day3-session4
make course-status
make test-observer
```

Expected untouched result: the Observer test stops at `TODO-D3.7` because the metrics observer has not yet been subscribed.

### Activity — Implement and Register the Metrics Observer

**File:** `src/gw_observers.c`  
**Functions:** `metrics_on_event`, `gw_observers_register`  
**TODO:** `TODO-D3.7`

Required behaviour:

- count every completed-event notification;
- increment transition count only for the transition outcome;
- increment alert count only from the committed result;
- register metrics in the documented observer order between the optional logger and reporter;
- do not free or retain borrowed event data.

Do not modify the generic Observer implementation, logger observer, reporter observer, binary observer, or tests.

Verify the focused functionality:

```sh
make test-observer
make test-diag-binary
make block4-cli-regression
```

Expected Observer evidence includes:

```text
[gateway] logger 5 lines, metrics {5, 2, 1}, reporter seq 4
test_observer: all groups passed (0 live)
```

The binary diagnostic suite must also pass all seven groups.

### Final Day 3 Gate

Run:

```sh
make day3-final-check
```

The final gate rechecks the prerequisite Day 1/Day 2 contracts, Day 3 file/process/thread/network work, Observer and binary diagnostics, CLI rejection paths, and final pipeline equivalence.

Required final line:

```text
DAY 3 FINAL CHECK: PASS - Valgrind optional where unavailable
```

---

## Day 3 completion rule

Do not move to the next session merely because the project compiles. Move on only when the current session's focused tests and stated evidence match this guide.

The final Day 3 claim is:

- Session 1: directory traversal, bounded paths, pipe I/O and descriptor ownership are correct;
- Session 2: queue synchronization, condition-variable shutdown and worker joins are correct;
- Session 3: TCP stream framing and the fixed-slot Reactor behave deterministically;
- Session 4: Observer metrics preserve pipeline semantics and persistence diagnostics remain valid;
- `make day3-final-check` passes.
