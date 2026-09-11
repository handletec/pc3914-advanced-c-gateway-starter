#ifndef EVENT_PARSER_H
#define EVENT_PARSER_H
#include "event.h"
#include <stddef.h>

/* Record format (one record per line, newline-terminated):
 *
 *     <module_id> <KIND> <rssi_dbm> <payload>
 *     3 ERROR -113 SIM_NOT_READY
 *
 * Exactly four fields separated by single spaces. The payload is the rest of
 * the line and must not contain a space. Blank lines are skipped. */

#define PARSE_FIELDS 4

typedef enum {
    PARSE_OK = 0,
    PARSE_ERR_FIELD_COUNT,     /* fewer or more than PARSE_FIELDS fields */
    PARSE_ERR_EMPTY_FIELD,     /* two consecutive delimiters */
    PARSE_ERR_BAD_NUMBER,      /* module_id or rssi_dbm is not a valid integer */
    PARSE_ERR_BAD_KIND         /* unknown event kind */
} parse_status_t;

/* A borrowed slice of the input: `len` bytes starting at `start`.
 * Not NUL-terminated. Valid only while the input buffer is. */
typedef struct {
    const char *start;
    size_t      len;
} field_view_t;

/* Delivered once per well-formed record. `ev` and ev->payload are valid only
 * until the callback returns. */
typedef void (*event_cb_t)(const event_view_t *ev);

typedef struct {
    size_t events;      /* well-formed records delivered to the callback */
    size_t rejected;    /* malformed records skipped */
    size_t consumed;    /* bytes consumed from buf; < len means an
                           incomplete (unterminated) record remains at the end */
} parse_result_t;

/* Parses every complete record in [buf, buf + len) and delivers each
 * well-formed one to cb as a borrowed view. Never reads at or beyond
 * buf + len. Bytes after the last newline are not consumed: the caller
 * keeps them and supplies them again with more data. */
parse_result_t parse_events(const char *buf, size_t len, event_cb_t cb);

/* Parses one record [rec, rec_end) — no newline inside — into *out.
 * On success out->payload points inside [rec, rec_end). */
parse_status_t parse_record(const char *rec, const char *rec_end, event_view_t *out);

/* Returns a pointer to the first `delim` in [p, end), or `end` if there is
 * none. Never dereferences `end`. Exposed for unit tests. */
const char *scan_until(const char *p, const char *end, char delim);

const char *parse_status_name(parse_status_t s);

#endif
