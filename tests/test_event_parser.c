/* test_event_parser.c — the parser's executable specification.
 *
 * Groups run in order; the first failing assert aborts the run and names
 * the TODO that fixes it. Run one group with: ./build/test_event_parser <group>
 *   groups: valid scan fields values records all */
#include "event_parser.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

/* ---- helpers ------------------------------------------------------- */

static parse_status_t parse_line(const char *line, event_view_t *out)
{
    return parse_record(line, line + strlen(line), out);
}

/* The buffer under test and what the callback must see. */
static const char *g_buf;
static size_t      g_len;
static size_t      g_count;

static const struct {
    uint32_t     module_id;
    event_kind_t kind;
    int16_t      rssi_dbm;
    const char  *payload;
} expected[] = {
    { 3, EV_ERROR,  -113, "SIM_NOT_READY" },
    { 3, EV_SIGNAL,  -71, "CONNECTED"     },
    { 1, EV_SIGNAL,  -80, "CONNECTED"     },
};

static void on_event(const event_view_t *ev)
{
    assert(g_count < sizeof expected / sizeof expected[0]);

    /* the view is BORROWED: it must point inside the buffer we supplied */
    assert(ev->payload >= g_buf);
    assert(ev->payload + ev->payload_len <= g_buf + g_len);

    assert(ev->module_id == expected[g_count].module_id);
    assert(ev->kind      == expected[g_count].kind);
    assert(ev->rssi_dbm  == expected[g_count].rssi_dbm);
    assert(ev->payload_len == strlen(expected[g_count].payload));
    assert(memcmp(ev->payload, expected[g_count].payload, ev->payload_len) == 0);
    g_count++;
}

/* ---- groups -------------------------------------------------------- */

static void test_valid(void)
{
    static const char text[] =
        "3 ERROR -113 SIM_NOT_READY\n"
        "3 SIGNAL -71 CONNECTED\n"
        "1 SIGNAL -80 CONNECTED\n";
    g_buf = text; g_len = sizeof text - 1; g_count = 0;

    parse_result_t r = parse_events(text, g_len, on_event);
    assert(r.events   == 3);
    assert(r.rejected == 0);
    assert(r.consumed == g_len);
    assert(g_count == 3);
    puts("[valid]   OK");
}

static void test_scan(void)
{
    /* 16 bytes under test, then padding. A newline is planted BEYOND the
     * bound so an unbounded scan returns a visible wrong answer instead of
     * reading unowned memory. */
    char pad[32];
    memset(pad, 'X', sizeof pad);
    memcpy(pad, "3 ERROR -113 SIM", 16);
    pad[20] = '\n';
    const char *end = pad + 16;

    assert(scan_until(pad, end, ' ') == pad + 1);            /* found: points AT the delimiter */
    assert(scan_until(pad, end, '\n') == end
           && "TODO-L1.1: return end when the delimiter is absent");
    assert(scan_until(end, end, ' ') == end);                /* empty range: nothing dereferenced */
    assert(scan_until(pad + 15, end, 'M') == pad + 15);      /* delimiter in the last byte */
    assert(scan_until(pad + 15, end, ' ') == end);           /* last byte is not the delimiter */
    puts("[scan]    OK");
}

static void test_fields(void)
{
    event_view_t ev;
    assert(parse_line("3 ERROR -113 SIM_NOT_READY", &ev) == PARSE_OK);
    assert(ev.payload_len == 13);

    assert(parse_line("3 ERROR -113", &ev) == PARSE_ERR_FIELD_COUNT
           && "TODO-L1.2b: too few fields");
    assert(parse_line("3 ERROR", &ev) == PARSE_ERR_FIELD_COUNT
           && "TODO-L1.2b: too few fields");
    assert(parse_line("3 ERROR -113 ", &ev) == PARSE_ERR_FIELD_COUNT
           && "TODO-L1.2b: no payload");
    assert(parse_line("3 ERROR -113 SIM_NOT_READY EXTRA", &ev) == PARSE_ERR_FIELD_COUNT
           && "TODO-L1.2c: too many fields");
    assert(parse_line("3  ERROR -113 SIM_NOT_READY", &ev) == PARSE_ERR_EMPTY_FIELD
           && "TODO-L1.2a: empty field");
    assert(parse_line("", &ev) == PARSE_ERR_FIELD_COUNT
           && "TODO-L1.2b: empty record");
    puts("[fields]  OK");
}

static void test_values(void)
{
    event_view_t ev;
    assert(parse_line("x ERROR -113 P", &ev) == PARSE_ERR_BAD_NUMBER);
    assert(parse_line("3 BOGUS -113 P", &ev) == PARSE_ERR_BAD_KIND);
    assert(parse_line("3 ERROR abc P",  &ev) == PARSE_ERR_BAD_NUMBER);
    assert(parse_line("3 ERROR -99999 P", &ev) == PARSE_ERR_BAD_NUMBER);  /* out of int16 range */
    assert(parse_line("3 ERROR -32768 P", &ev) == PARSE_OK);
    assert(ev.rssi_dbm == -32768);
    assert(parse_line("4294967295 SIGNAL -71 P", &ev) == PARSE_OK);       /* UINT32_MAX */
    assert(parse_line("4294967296 SIGNAL -71 P", &ev) == PARSE_ERR_BAD_NUMBER);
    puts("[values]  OK");
}

static void dummy_cb(const event_view_t *ev) { (void)ev; }

static void test_records(void)
{
    /* truncated input: the unterminated tail is not consumed */
    static const char t1[] = "3 ERROR -113 SIM_NOT_READY\n3 SIGNAL -71 CONNEC";
    parse_result_t r = parse_events(t1, sizeof t1 - 1, dummy_cb);
    assert(r.events == 1 && r.rejected == 0);
    assert(r.consumed == 27);                       /* through the first newline only */

    /* malformed record in the middle is skipped, parsing continues */
    static const char t2[] = "3 ERROR -113 SIM_NOT_READY\n3 ERROR -113\n1 SIGNAL -80 CONNECTED\n";
    r = parse_events(t2, sizeof t2 - 1, dummy_cb);
    assert(r.events == 2 && r.rejected == 1);
    assert(r.consumed == sizeof t2 - 1);

    /* blank lines are skipped, not rejected */
    static const char t3[] = "\n3 SIGNAL -71 CONNECTED\n\n";
    r = parse_events(t3, sizeof t3 - 1, dummy_cb);
    assert(r.events == 1 && r.rejected == 0);

    /* empty input */
    r = parse_events(t3, 0, dummy_cb);
    assert(r.events == 0 && r.consumed == 0);
    puts("[records] OK");
}

/* ---- runner -------------------------------------------------------- */

int main(int argc, char **argv)
{
    setvbuf(stdout, NULL, _IONBF, 0);       /* keep group lines visible before an abort */
    const char *g = argc > 1 ? argv[1] : "all";
    int all = strcmp(g, "all") == 0;

    if (all || !strcmp(g, "valid"))   test_valid();
    if (all || !strcmp(g, "scan"))    test_scan();
    if (all || !strcmp(g, "fields"))  test_fields();
    if (all || !strcmp(g, "values"))  test_values();
    if (all || !strcmp(g, "records")) test_records();

    puts("test_event_parser: all groups passed");
    return 0;
}
