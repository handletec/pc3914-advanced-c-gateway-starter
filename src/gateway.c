/* gateway.c — Wireless Module Diagnostics Gateway, Day 1 starting point.
 *
 *   Simulated Module Events (file) -> Event Parser -> Diagnostic Engine -> Console
 *
 * Input is read in fixed-size chunks into ONE buffer that is reused for every
 * read. A record split across two chunks is carried over: the parser reports
 * how many bytes it consumed and the unconsumed tail is moved to the front. */
#include "event_parser.h"
#include "diag_engine.h"
#include <stdio.h>
#include <string.h>

#define READ_CHUNK 4096

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s <events-file>\n", argv[0]);
        return 2;
    }
    FILE *in = fopen(argv[1], "rb");
    if (!in) {
        perror(argv[1]);
        return 1;
    }

    char   buf[READ_CHUNK];
    size_t held = 0;                        /* bytes in buf not yet consumed */
    parse_result_t total = { 0, 0, 0 };

    for (;;) {
        size_t n = fread(buf + held, 1, sizeof buf - held, in);
        if (n == 0)
            break;
        held += n;

        parse_result_t r = parse_events(buf, held, diag_on_event);
        total.events   += r.events;
        total.rejected += r.rejected;

        /* keep the unterminated tail; it is completed by the next read */
        memmove(buf, buf + r.consumed, held - r.consumed);
        held -= r.consumed;

        if (held == sizeof buf) {
            fprintf(stderr, "error: record longer than %d bytes\n", READ_CHUNK);
            fclose(in);
            return 1;
        }
    }
    fclose(in);

    if (held > 0)
        fprintf(stderr, "warning: %zu bytes of incomplete record at end of input\n", held);

    diag_report();
    printf("parser: %zu events, %zu rejected\n", total.events, total.rejected);
    return 0;
}
