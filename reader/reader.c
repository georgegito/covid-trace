/* -------------------------------------------------------------------------- */
/*                                  reader.c                                  */
/* -------------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <pthread.h>
#include <headers.h>
#include <covid_trace.c>
#include <utils.c>
#include <queue.c>

int main()
{
    /* ----------------------------- print contacts ----------------------------- */
    printf("\nuploaded_contacts:\n\n");
    read_cont_bin();

    /* -------------------------- print bt search times ------------------------- */
    printf("\nbt_search_times:\n\n");
    read_bt_search_times_bin();

    /* -------------------------------------------------------------------------- */
    return 0;
}