#ifndef EVENT_H
#define EVENT_H
#include <stddef.h>
#include <stdint.h>

/* Event kinds reported by a wireless module (simulated). */
typedef enum {
    EV_CONNECT,
    EV_DISCONNECT,
    EV_SIGNAL,
    EV_ERROR,
    EV_KIND_COUNT
} event_kind_t;

/* One parsed event record.
 *
 * `payload` is BORROWED: it points into the input buffer that was passed to
 * parse_events(). It is valid only while that buffer still holds the same
 * bytes — in this gateway, only for the duration of the callback.
 * It is NOT NUL-terminated; always use payload_len (print with "%.*s").
 * Nothing in this struct is allocated and nothing needs to be freed. */
typedef struct {
    uint32_t     module_id;
    event_kind_t kind;
    int16_t      rssi_dbm;
    const char  *payload;
    size_t       payload_len;
} event_view_t;

/* Static name for a kind ("CONNECT", "SIGNAL", ...). Never NULL. */
const char *event_kind_name(event_kind_t k);

#endif
