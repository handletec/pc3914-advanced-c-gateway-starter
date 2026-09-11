#ifndef DIAG_ENGINE_H
#define DIAG_ENGINE_H
#include "event.h"

/* Day 1 morning engine: prints each event and keeps simple counters.
 * It retains no pointers from the events it receives. */
void diag_on_event(const event_view_t *ev);   /* parse_events callback */
void diag_report(void);

#endif
