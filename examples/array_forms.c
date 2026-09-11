/* array_forms.c — three ways to hold the four fields of one record.
 * Side example for the "advanced arrays" discussion. Build: make demo-arrays */
#include "event_parser.h"
#include <stdio.h>
#include <string.h>

int main(void)
{
    static const char rec[] = "3 ERROR -113 SIM_NOT_READY";
    const char *rec_end = rec + sizeof rec - 1;

    /* Form A: fixed two-dimensional storage. Copies. Every field costs 32
     * bytes whether it needs 1 or 31. The array OWNS the characters. */
    char fixed[PARSE_FIELDS][32];

    /* Form B: array of pointers into the record. No copy. Any length.
     * The characters stay in `rec`; this array owns only four pointers. */
    const char *ptrs[PARSE_FIELDS];

    /* Form C (the project): pointer + length. No terminator needed,
     * so no copy and no writing into the input. */
    field_view_t views[PARSE_FIELDS];

    const char *p = rec;
    for (int i = 0; i < PARSE_FIELDS; i++) {
        const char *sp = (i < PARSE_FIELDS - 1) ? scan_until(p, rec_end, ' ') : rec_end;
        size_t len = (size_t)(sp - p);

        memcpy(fixed[i], p, len < 31 ? len : 31);   /* A: bounded copy, then terminate */
        fixed[i][len < 31 ? len : 31] = '\0';

        ptrs[i]  = p;                               /* B: cannot say where the field ends
                                                          without the delimiter still in place */
        views[i] = (field_view_t){ p, len };        /* C */

        p = (sp < rec_end) ? sp + 1 : rec_end;
    }

    printf("record: \"%s\"\n\n", rec);
    printf("form                  sizeof   step from [1] to [2]   field[1]\n");
    printf("A char[4][32]         %4zu     %3td bytes              %s\n",
           sizeof fixed, (char *)fixed[2] - (char *)fixed[1], fixed[1]);
    printf("B const char *[4]     %4zu     %3td bytes              %.*s\n",
           sizeof ptrs,  (char *)&ptrs[2] - (char *)&ptrs[1],
           (int)(ptrs[2] - ptrs[1] - 1), ptrs[1]);
    printf("C field_view_t[4]     %4zu     %3td bytes              %.*s\n",
           sizeof views, (char *)&views[2] - (char *)&views[1],
           (int)views[1].len, views[1].start);

    printf("\nfixed[1]  is char[32] starting %td bytes into the block\n",
           (char *)fixed[1] - (char *)fixed);
    printf("ptrs[1]   is a pointer to byte %td of rec\n", ptrs[1] - rec);
    printf("views[1]  is {rec + %td, %zu}\n", views[1].start - rec, views[1].len);
    return 0;
}
