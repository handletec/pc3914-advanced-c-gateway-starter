#include "event.h"

static const char *const kind_names[EV_KIND_COUNT] = {
    [EV_CONNECT]    = "CONNECT",
    [EV_DISCONNECT] = "DISCONNECT",
    [EV_SIGNAL]     = "SIGNAL",
    [EV_ERROR]      = "ERROR",
};

const char *event_kind_name(event_kind_t k)
{
    return (k >= 0 && k < EV_KIND_COUNT) ? kind_names[k] : "?";
}
