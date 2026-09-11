# PC3914 Advanced C Gateway Starter

Participant starter repository for **PC3914 - Advanced C Programming: From Basic C to Systems-Ready**, material version **v2.17**.

This is a simulated engineering project used for training. It does not represent a real product architecture.

## Environment

Use an Ubuntu/Linux environment. See [`docs/SETUP.md`](docs/SETUP.md) for Ubuntu, Windows/WSL2, and macOS/Ubuntu-VM setup instructions.

Before beginning the practical work, run:

```sh
./tools/pc3914_readiness_check_v2.17.sh
```

The required final line is:

```text
PC3914 READINESS: PASS
```

## Project flow

```text
Simulated Module Events (data/*.txt)
        |
        v
    Event Parser        src/event_parser.c
        |
        v
 Diagnostic Engine      src/diag_engine.c
        |
        v
  Console Output        src/gateway.c
```

## Build and run

```sh
make clean
make
make run
make test-baseline
```

Compiler flags are `gcc -std=c11 -Wall -Wextra -Werror -g -O0`.

`make test-baseline` must pass in the untouched starter. The complete `make test` command is expected to stop on the first unfinished exercise until the current TODOs are implemented correctly.

## Record format

```text
<module_id> <KIND> <rssi_dbm> <payload>
3 ERROR -113 SIM_NOT_READY
```

Records contain four single-space-separated fields. `KIND` is one of `CONNECT`, `DISCONNECT`, `SIGNAL`, or `ERROR`. The payload is one token.

## Initial exercises

| TODO | File | Goal | Test group |
|---|---|---|---|
| `TODO-L1.1` | `src/event_parser.c` | Implement bounded `scan_until` without reading past `end` | `scan` |
| `TODO-L1.2` | `src/event_parser.c` | Split exactly four fields and reject malformed records | `fields` |

Run one parser test group with:

```sh
./build/test_event_parser scan
```

## Data files

- `data/module_events.txt` - 12 well-formed records used by the baseline demo.
- `data/malformed_events.txt` - malformed input cases.
- `data/truncated_events.txt` - final record has no newline.
- `data/burst_events.txt` - input larger than one 4 KiB read chunk.

## Repository scope

This repository intentionally contains only the **initial participant starter state**. It contains no completed exercise implementation, answer key, private reference implementation, or later known-good classroom state.
