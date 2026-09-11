/* diag_engine.c — Day 1 morning: print every event, keep simple counters.
 * Deliberately retains nothing from the event views it receives. */
#include "diag_engine.h"
#include <stdio.h>

#define DIAG_MAX_MODULES 8

static struct {
    size_t   total;
    size_t   errors;
    int16_t  weakest_rssi;
    uint32_t weakest_module;
    int      have_signal;
} stats;

static unsigned char seen[DIAG_MAX_MODULES];

void diag_on_event(const event_view_t *ev)
{
    /* payload is not NUL-terminated: print exactly payload_len bytes */
    printf("[module %u] %-10s %5d dBm  %.*s\n",
           ev->module_id, event_kind_name(ev->kind), ev->rssi_dbm,
           (int)ev->payload_len, ev->payload);

    stats.total++;
    if (ev->kind == EV_ERROR)
        stats.errors++;
    if (ev->kind == EV_SIGNAL &&
        (!stats.have_signal || ev->rssi_dbm < stats.weakest_rssi)) {
        stats.weakest_rssi   = ev->rssi_dbm;
        stats.weakest_module = ev->module_id;
        stats.have_signal    = 1;
    }
    if (ev->module_id < DIAG_MAX_MODULES)
        seen[ev->module_id] = 1;
}

void diag_report(void)
{
    unsigned modules = 0;
    for (unsigned i = 0; i < DIAG_MAX_MODULES; i++)
        modules += seen[i];

    printf("--- diagnostics ---\n");
    printf("events: %zu  errors: %zu  modules seen: %u\n",
           stats.total, stats.errors, modules);
    if (stats.have_signal)
        printf("weakest signal: module %u at %d dBm\n",
               stats.weakest_module, stats.weakest_rssi);
}
