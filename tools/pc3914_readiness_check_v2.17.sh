#!/bin/sh
# PC3914 participant readiness check - material v2.17
# This script makes no changes to the machine.
set -u

pass=0
fail=0
warn=0
say_pass(){ printf 'PASS  %s\n' "$1"; pass=$((pass+1)); }
say_fail(){ printf 'FAIL  %s\n' "$1"; fail=$((fail+1)); }
say_warn(){ printf 'WARN  %s\n' "$1"; warn=$((warn+1)); }

printf '%s\n' 'PC3914 - Advanced C Programming: From Basic C to Systems-Ready'
printf '%s\n' 'Participant readiness check v2.17'
printf '%s\n\n' '------------------------------------------------------------'

os=$(uname -s 2>/dev/null || printf unknown)
if [ "$os" = Linux ]; then
  say_pass 'Linux environment detected'
else
  say_fail "Linux environment required for governed lab evidence (detected: $os)"
fi

for c in gcc make gdb valgrind strace ar nm ldd tar gzip grep sed awk diff sort tail timeout sha256sum find mktemp mv; do
  if command -v "$c" >/dev/null 2>&1; then
    say_pass "$c available"
  else
    say_fail "$c missing"
  fi
done

work=${TMPDIR:-/tmp}/pc3914-readiness-$$
mkdir -p "$work" || exit 2
trap 'rm -rf "$work"' EXIT HUP INT TERM
cat > "$work/smoke.c" <<'C'
#define _POSIX_C_SOURCE 200809L
#include <poll.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
static void *worker(void *p){ return p; }
int main(void){
    int p[2];
    int s[2];
    pthread_t t;
    struct pollfd fd = { .fd = -1, .events = POLLIN };
    if (pipe(p) != 0) return 1;
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, s) != 0) return 2;
    if (pthread_create(&t, 0, worker, 0) != 0) return 3;
    if (pthread_join(t, 0) != 0) return 4;
    (void)poll(&fd, 0, 0);
    pid_t pid=fork();
    if (pid < 0) return 5;
    if (pid == 0) _exit(0);
    if (waitpid(pid, 0, 0) != pid) return 6;
    close(p[0]); close(p[1]); close(s[0]); close(s[1]);
    return 0;
}
C
if command -v gcc >/dev/null 2>&1 && gcc -std=c11 -Wall -Wextra -Werror -pthread "$work/smoke.c" -o "$work/smoke" >/dev/null 2>&1; then
  say_pass 'C11/POSIX/pthread compile smoke test'
  if "$work/smoke" >/dev/null 2>&1; then say_pass 'process/pipe/poll/socket/pthread runtime smoke test'; else say_fail 'POSIX runtime smoke test'; fi
else
  say_fail 'C11/POSIX/pthread compile smoke test'
fi

printf '\nSummary: %d PASS, %d WARN, %d FAIL\n' "$pass" "$warn" "$fail"
if [ "$fail" -eq 0 ]; then
  printf '%s\n' 'PC3914 READINESS: PASS'
  exit 0
fi
printf '%s\n' 'PC3914 READINESS: ACTION REQUIRED'
exit 1
