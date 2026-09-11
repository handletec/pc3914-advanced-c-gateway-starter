/* event_parser.c — bounded parser producing borrowed views.
 *
 * Participants complete TODO-L1.1 and TODO-L1.2. Everything else is prepared.
 * No function in this file allocates memory. */
#include "event_parser.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* ------------------------------------------------------------------ */
/* Bounded scanning                                                    */
/* ------------------------------------------------------------------ */

/* TODO-L1.1: bounded scan.
 * This version stops only when it sees `delim`. If the delimiter is absent
 * it keeps reading past `end` — that is undefined behaviour.
 * Required: never read at or beyond `end`; return `end` when the delimiter
 * is not found. The parser tests in tests/test_event_parser.c define the
 * exact behaviour. */
const char *scan_until(const char *p, const char *end, char delim)
{
    (void)end;                      /* remove once `end` is used */
    while (*p != delim)
        ++p;
    return p;
}

/* Extracts the next space-delimited field starting at *cursor.
 * On return *cursor is just past the delimiter (or == end if none was found).
 * Returns false when there is nothing left to read. */
static bool next_field(const char **cursor, const char *end, field_view_t *out)
{
    const char *p = *cursor;
    if (p >= end)
        return false;

    const char *sp = scan_until(p, end, ' ');
    out->start = p;
    out->len   = (size_t)(sp - p);

    *cursor = (sp < end) ? sp + 1 : end;    /* moves the caller's pointer */
    return true;
}

/* TODO-L1.2: split one record into exactly PARSE_FIELDS borrowed views.
 * The first PARSE_FIELDS - 1 fields are space-delimited tokens; the last
 * field (payload) is the rest of the record.
 * This version accepts anything. Required:
 *   L1.2a  an empty field                         -> PARSE_ERR_EMPTY_FIELD
 *   L1.2b  too few fields / no payload            -> PARSE_ERR_FIELD_COUNT
 *   L1.2c  payload containing a space (5th field) -> PARSE_ERR_FIELD_COUNT */
static parse_status_t split_fields(const char *rec, const char *rec_end,
                                   field_view_t fields[PARSE_FIELDS])
{
    const char *p = rec;                    /* our position; next_field moves it */
    size_t n = 0;

    while (n < PARSE_FIELDS - 1 && next_field(&p, rec_end, &fields[n])) {
        /* TODO-L1.2a */
        n++;
    }

    /* TODO-L1.2b */

    fields[n].start = p;                    /* payload: the rest of the record */
    fields[n].len   = (size_t)(rec_end - p);

    /* TODO-L1.2c */

    return PARSE_OK;
}

/* ------------------------------------------------------------------ */
/* Field conversion (prepared)                                         */
/* ------------------------------------------------------------------ */

/* Views are not C strings: no terminator, so strtol() is not usable.
 * These walk [start, start + len) and nothing else. */
static bool parse_u32_field(field_view_t f, uint32_t *out)
{
    const char *p = f.start, *end = f.start + f.len;
    if (p == end)
        return false;
    uint32_t v = 0;
    for (; p < end; ++p) {
        if (*p < '0' || *p > '9')
            return false;
        unsigned d = (unsigned)(*p - '0');
        if (v > (UINT32_MAX - d) / 10)
            return false;
        v = v * 10 + d;
    }
    *out = v;
    return true;
}

static bool parse_i16_field(field_view_t f, int16_t *out)
{
    const char *p = f.start, *end = f.start + f.len;
    bool neg = false;
    if (p < end && (*p == '-' || *p == '+')) {
        neg = (*p == '-');
        ++p;
    }
    if (p == end)
        return false;
    int32_t v = 0;
    for (; p < end; ++p) {
        if (*p < '0' || *p > '9')
            return false;
        v = v * 10 + (*p - '0');
        if (v > 32768)
            return false;
    }
    if (neg)
        v = -v;
    if (v < INT16_MIN || v > INT16_MAX)
        return false;
    *out = (int16_t)v;
    return true;
}

static bool parse_kind_field(field_view_t f, event_kind_t *out)
{
    for (int k = 0; k < EV_KIND_COUNT; k++) {
        const char *name = event_kind_name((event_kind_t)k);
        size_t n = strlen(name);
        if (f.len == n && memcmp(f.start, name, n) == 0) {
            *out = (event_kind_t)k;
            return true;
        }
    }
    return false;
}

/* ------------------------------------------------------------------ */
/* Records                                                             */
/* ------------------------------------------------------------------ */

parse_status_t parse_record(const char *rec, const char *rec_end, event_view_t *out)
{
    field_view_t f[PARSE_FIELDS] = {{0, 0}};

    parse_status_t st = split_fields(rec, rec_end, f);
    if (st != PARSE_OK)
        return st;

    if (!parse_u32_field(f[0], &out->module_id))
        return PARSE_ERR_BAD_NUMBER;
    if (!parse_kind_field(f[1], &out->kind))
        return PARSE_ERR_BAD_KIND;
    if (!parse_i16_field(f[2], &out->rssi_dbm))
        return PARSE_ERR_BAD_NUMBER;

    out->payload     = f[3].start;          /* borrowed: inside [rec, rec_end) */
    out->payload_len = f[3].len;
    return PARSE_OK;
}

parse_result_t parse_events(const char *buf, size_t len, event_cb_t cb)
{
    parse_result_t r = { 0, 0, 0 };
    const char *p   = buf;
    const char *end = buf + len;            /* one past the last byte: compare, never read */

    while (p < end) {
        const char *nl = scan_until(p, end, '\n');
        if (nl == end)
            break;                          /* unterminated record: leave it for the caller */

        if (nl > p) {                       /* skip blank lines */
            event_view_t ev;
            if (parse_record(p, nl, &ev) == PARSE_OK) {
                cb(&ev);                    /* ev.payload valid only inside this call */
                r.events++;
            } else {
                r.rejected++;
            }
        }
        p = nl + 1;
        r.consumed = (size_t)(p - buf);
    }
    return r;
}

const char *parse_status_name(parse_status_t s)
{
    switch (s) {
    case PARSE_OK:              return "OK";
    case PARSE_ERR_FIELD_COUNT: return "FIELD_COUNT";
    case PARSE_ERR_EMPTY_FIELD: return "EMPTY_FIELD";
    case PARSE_ERR_BAD_NUMBER:  return "BAD_NUMBER";
    case PARSE_ERR_BAD_KIND:    return "BAD_KIND";
    }
    return "?";
}
