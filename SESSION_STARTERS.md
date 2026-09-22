# PC3914 Session Starters

The files in `session-starters/` are the governed PC3914 v2.17 participant session-entry archives. They are stored in Git exactly so the classroom helper can verify and materialize the correct cumulative state.

Do not edit or unpack these archives manually during normal course delivery. Use:

```sh
./tools/start-session <session-id>
```

| Session ID | Session focus | Governed archive |
| --- | --- | --- |
| `day1-session1` | Bounded parsing and borrowed views | `PC3914_Day1_Session1_Participant_Starter_v2.17.tar.gz` |
| `day1-session2` | Dynamic allocation, ownership and retained lifetime | `PC3914_Day1_Session2_Participant_Starter_v2.17.tar.gz` |
| `day1-session3` | Arena allocation, batch lifetime and debugging evidence | `PC3914_Day1_Session3_Participant_Starter_v2.17.tar.gz` |
| `day1-session4` | Regression challenges and cumulative Day 1 verification | `PC3914_Day1_Session4_Participant_Starter_v2.17.tar.gz` |
| `day2-session1` | Linear structures | `PC3914_Day2_Session1_Participant_Starter_v2.17.tar.gz` |
| `day2-session2` | Non-linear structures and invariants | `PC3914_Day2_Session2_Participant_Starter_v2.17.tar.gz` |
| `day2-session3` | Opaque modules and libraries | `PC3914_Day2_Session3_Participant_Starter_v2.17.tar.gz` |
| `day2-session4` | Replaceable behaviour | `PC3914_Day2_Session4_Participant_Starter_v2.17.tar.gz` |
| `day3-session1` | Files, processes and pipes | `PC3914_Day3_Session1_Participant_Starter_v2.17.tar.gz` |
| `day3-session2` | Threads, thread-safe work queue and worker shutdown | `PC3914_Day3_Session2_Participant_Starter_v2.17.tar.gz` |
| `day3-session3` | TCP clients, stream framing and one `poll()` Reactor | `PC3914_Day3_Session3_Participant_Starter_v2.17.tar.gz` |
| `day3-session4` | Observer pattern, persistence and final integration | `PC3914_Day3_Session4_Participant_Starter_v2.17.tar.gz` |

## Integrity

`session-starters/SHA256SUMS` records the governed SHA-256 for all 12 archives. `tools/start-session` refuses extraction if the selected archive does not match its recorded digest.

Every later archive contains the completed prerequisite course state plus only the intended new session problem state. These are participant-safe starting checkpoints, not solution packages or restore controls.

## Day 1 execution guide

For the exact Day 1 GDB and Valgrind/Memcheck workflows—including how to build and diagnose the prepared `gateway_broken` binary—see [`DAY1_PARTICIPANT_GUIDE.md`](DAY1_PARTICIPANT_GUIDE.md).

## Day 2 execution guide

For the participant-facing Day 2 sequence—first command, expected starter evidence, file/TODO boundary, verification commands and done condition—see [`DAY2_PARTICIPANT_GUIDE.md`](DAY2_PARTICIPANT_GUIDE.md).
