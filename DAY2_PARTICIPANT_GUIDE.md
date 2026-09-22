# PC3914 Day 2 Participant Activity Guide

This guide is the classroom navigation for **Day 2** of PC3914 v2.17.

The session starter archives are governed and must not be edited. Use the repository helper to create a separate workspace for each session, then do all exercise work inside that workspace.

## Before Day 2

From the repository root:

```sh
git status
git pull
```

If `git pull` refuses because tracked local changes would be overwritten, keep that folder as your previous work and make a fresh clone. Do not force/reset/stash/merge during class.

For each Day 2 session:

```sh
./tools/start-session <session-id>
cd work/<session-id>
make course-status
```

The untouched starter is expected to stop at the current exercise boundary. That failure is the starting evidence, not a broken package.

Before a substantial edit you may save your current session tree with:

```sh
make checkpoint
```

---

## Day 2 Session 1 — Linear Structures

### Start the session

From the repository root:

```sh
./tools/start-session day2-session1
cd work/day2-session1
make course-status
make test-day2
```

Expected untouched result: the queue suite stops at the TODO-D2.1 invariant:

```text
TODO-D2.1a: tail must be the last node (NULL when empty)
```

### Activity 1 — Complete Pop and Wire the Intake

**First file:** `src/event_queue.c`  
**Function:** `event_queue_pop`  
**TODO:** `TODO-D2.1`

Read the invariant comment immediately above the function before editing. Repair only the pop-side invariants: empty/single-node state, size accounting, node lifetime, and transfer of the event to the caller.

Do not edit `event_queue_push` or the tests.

Verify the queue first:

```sh
make test-queue
```

Next open:

- `src/gateway.c::on_event` — `TODO-D2.2a`
- `src/gateway.c::process_intake` — `TODO-D2.2b`

The arrival stage must transfer each successfully cloned event to the intake queue. The processing stage must drain the queue into the Event Store. Ownership transfers only when the receiving operation succeeds.

Build and run the integrated burst case:

```sh
make
./gateway data/burst_events.txt
```

Required evidence includes:

```text
intake: 400 queued, 400 processed, 0 still queued
heap: ... 0 live at exit
```

### Activity 2 — Remove a Sink by ID

**File:** `src/diag_sink_list.c`  
**Function:** `diag_sink_list_remove`  
**TODO:** `TODO-D2.3`

Use one pointer-to-pointer traversal so head, middle, last and missing-ID cases use the same unlink rule. Do not change `diag_sink_list_add` or the tests.

Verify:

```sh
make test-sinks
make test-day2
```

Done for Session 1 when queue, sink and stack tests pass and the burst run reports **400 queued / 400 processed / 0 still queued** with **0 live blocks**.

Final course-environment gate:

```sh
make day2-block1-check
```

This final gate uses Valgrind. If the command itself is missing, that is an environment/readiness problem; do not change exercise code to bypass it.

---

## Day 2 Session 2 — Non-Linear Structures and Invariants

### Start the session

From the repository root:

```sh
./tools/start-session day2-session2
cd work/day2-session2
make course-status
make test-day2-block2
```

Expected untouched result:

```text
TODO-D2.4: a colliding id must be probed to the next free slot, not overwrite
```

### Activity 1 — Implement Linear Probing

**File:** `src/device_registry.c`  
**Function:** `find_slot`  
**TODO:** `TODO-D2.4`

Change only the slot-search logic. Start from the home slot, probe with wraparound, stop at a matching ID or the first empty slot, and bound the search by the table capacity.

Do not change `grow`, `device_registry_upsert`, the hash function, or the tests.

Verify:

```sh
make test-registry
```

Required result:

```text
[basic]     OK
[collision] OK
[growth]    OK
```

### Activity 2 — Implement Sift-Down

**File:** `src/alert_heap.c`  
**Function:** `sift_down`  
**TODO:** `TODO-D2.5`

Use the existing `alert_outranks` rule. Compare only children whose indexes are inside `h->len`, choose the strongest candidate, swap when needed, and continue until the invariant holds.

Do not change `sift_up`, `alert_outranks`, pop semantics, or the tests.

Verify:

```sh
make test-heap
make test-bst
make test-day2-block2
```

Then run the integrated case:

```sh
make
./gateway data/burst_events.txt
```

Required evidence includes:

```text
devices: 4 tracked (registry capacity 16)
alerts: 154 raised
heap: ... 0 live at exit
```

The first alerts should be severity 3 and follow the existing deterministic sequence tie-break.

Final course-environment gate:

```sh
make day2-block2-check
```

---

## Day 2 Session 3 — Opaque Modules and Libraries

### Start the session

From the repository root:

```sh
./tools/start-session day2-session3
cd work/day2-session3
make course-status
make opaque-check
```

Expected untouched result:

```text
test_registry_opaque: public interface only — OK
OPAQUE CHECK: FAIL — private access still compiles
```

### Activity — Hide the Registry

Work through these locations in order:

1. `include/device_registry.h` — `TODO-D2.6a`
2. `src/device_registry.c` — `TODO-D2.6a`
3. `src/gateway.c` — `TODO-D2.6b`

The public header must expose an incomplete `device_registry_t` handle rather than the private slot/layout representation. Move the representation into the implementation file. Then replace the gateway's direct field traversal with the existing public iteration API.

Do not modify `tests/test_registry_opaque.c` to make the check pass.

Verify:

```sh
make opaque-check
make test-registry
make
```

The intended `opaque-check` result is:

```text
test_registry_opaque: public interface only — OK
OPAQUE CHECK: PASS — private access rejected by the compiler
... error: invalid use of incomplete typedef 'device_registry_t' ...
```

The compile error is expected evidence: it proves a caller cannot access the private representation.

### Lab — Build and Consume Gateway Core

The lab deliberately includes one **link-time** failure and one **runtime loader** failure. Record both before repairing them.

Create the lab workspace:

```sh
rm -rf build/lab
mkdir -p build/lab/obj build/lab/pic

CORE="event event_parser event_store event_queue diag_sink_list work_stack device_registry alert_heap event_bst arena diag_snapshot gw_alloc gw_status"
```

Build ordinary objects and the static archive:

```sh
for x in $CORE; do
  gcc -std=c11 -Wall -Wextra -Werror -g -O0 -Iinclude \
    -c src/$x.c -o build/lab/obj/$x.o
done

ar rcs build/lab/libgateway_core.a build/lab/obj/*.o
ar t build/lab/libgateway_core.a
```

Now intentionally omit the archive:

```sh
gcc -std=c11 -Wall -Wextra -Werror -g -O0 -Iinclude \
  examples/registry_client.c -o build/lab/registry_client_missing
```

This command is **expected to fail** with linker messages such as `undefined reference`. Do not edit source to remove those calls.

Repair the link by supplying the archive:

```sh
gcc -std=c11 -Wall -Wextra -Werror -g -O0 -Iinclude \
  examples/registry_client.c build/lab/libgateway_core.a \
  -o build/lab/registry_client

./build/lab/registry_client
```

Required first line:

```text
registry_client: 4 devices, capacity 8
```

Build PIC objects and the shared library:

```sh
for x in $CORE; do
  gcc -std=c11 -Wall -Wextra -Werror -g -O0 -Iinclude -fPIC \
    -c src/$x.c -o build/lab/pic/$x.o
done

gcc -shared -o build/lab/libgateway_core.so build/lab/pic/*.o

gcc -std=c11 -Wall -Wextra -Werror -g -O0 -Iinclude \
  examples/registry_client.c -Lbuild/lab -lgateway_core \
  -o build/lab/registry_client_shared
```

Run it once without loader configuration:

```sh
./build/lab/registry_client_shared
```

On the governed Linux environment this is expected to fail before `main` with a message like:

```text
error while loading shared libraries: libgateway_core.so: cannot open shared object file
```

Now configure runtime discovery:

```sh
LD_LIBRARY_PATH=build/lab ./build/lab/registry_client_shared
```

Required first line again:

```text
registry_client: 4 devices, capacity 8
```

Verify the lab artefacts:

```sh
make lab-check
```

Required final line:

```text
LINK LAB: PASS
```

Then run the session gate:

```sh
make day2-block3-check
```

---

## Day 2 Session 4 — Replaceable Behaviour

### Start the session

From the repository root:

```sh
./tools/start-session day2-session4
cd work/day2-session4
make course-status
make test-day2-block4
```

Expected untouched result:

```text
TODO-D2.7: conservative treats every ERROR as severity 3
```

### Activity — Add the Conservative Alert Policy

**File:** `src/alert_strategy.c`  
**Function:** `classify_conservative`  
**TODO:** `TODO-D2.7`

Implement only the new policy behind the existing Strategy interface. Use the supplied thresholds/constants and `fill_alert`. Do not add policy-specific branches to `gateway.c`, and do not change the heap.

Verify:

```sh
make test-strategy
make policy-compare
```

Expected policy comparison includes:

```text
--policy default      -> alerts: 154 raised
--policy conservative -> alerts: 180 raised
```

### Lab — Implement Module Lifecycle Transitions

**File:** `src/module_state_machine.c`  
**TODOs:** `TODO-D2.8a`, `TODO-D2.8b`

First read the lifecycle diagram and outcome policy in:

```text
include/module_state_machine.h
```

Then:

- implement the checked table lookup in `module_sm_next`;
- complete only the missing ONLINE and DEGRADED table rows;
- preserve the distinction between transition, defined stay, and rejected input;
- leave `*next` untouched on rejection.

Do not replace the table with a large branch/switch implementation and do not edit the tests.

Verify:

```sh
make test-state
```

Required table evidence:

```text
[table]      OK (8 transitions, 8 no-ops, 4 rejections)
test_module_state_machine: all groups passed
```

Now verify both Day 2 Block 4 mechanisms together:

```sh
make test-day2-block4
make trace-check
```

Expected trace result:

```text
TRACE CHECK: PASS
```

Finally, in the course environment:

```sh
make day2-block4-check
make day2-final-check
```

`day2-final-check` must end with:

```text
DAY 2 FINAL CHECK: PASS
```

---

## Day 2 completion rule

Do not move to the next session because the source merely compiles. Move on only when the current session's focused tests and stated evidence match this guide.

The final Day 2 claim is:

- Session 1: queue ownership/invariants and linked-list removal are correct;
- Session 2: registry probing and heap ordering invariants are correct;
- Session 3: registry representation is private and the core can be consumed as static/shared libraries;
- Session 4: alert policy is replaceable and lifecycle rules are table-driven;
- `make day2-final-check` passes in the governed course environment.
