/******************************************************************************
        Car Wash Model - C Implementation
******************************************************************************/

#include <stdio.h>
#include <malloc.h>
#include <math.h>

#include "proofmac.h"

#define TRUE    1
#define FALSE   0

#define NORMAL  0       /* Normal return from an event routine */
#define EXIT    1       /* Stop simulation */
#define BLOCKED 2       /* Event couldn't happen; retry later */

int     arrival(long int),              /* Event routines */
        start_wash(long int),
        start_dry(long int),
        exit_sys(long int),
        outta_here(long int),
        no_next_event(long int);        /* Dummy */

double  rvexpo(double), uniform();

struct event_notice
        {
        struct event_notice     *next_event,            /* Doubly linked list */
                                *prev_event;

        int                     (*event_routine) ();    /* Routine to invoke */
        long int                id_number;
        double                  event_time;
        };

static struct event_notice      zeroeth_event =
        {
        NULL,
        NULL,
        NULL,
        0,
        -999.99
        };

static struct event_notice      n_plus_1st_event =
        {
        NULL,
        NULL,
        NULL,
        0,
        999999999.999
        };

static double   time = 0.0,
                last_time_written = 0.0;

static int      washer_busy = FALSE;            /* TRUE/FALSE (single server) */
static int      termination_counter = 100;

static FILE     *trace_file;

main()
        {
        zeroeth_event.next_event    = &n_plus_1st_event;
        n_plus_1st_event.prev_event = &zeroeth_event;

        trace_file = fopen("carwash.atf", "w");

        schedule(&arrival, 1L, rvexpo(2.2));    /* Seed first arrival */

        simulate();

        fclose(trace_file);

        exit(0);
        }

schedule(routine, id, when)

        int     (*routine) ();                  /* Event routine */
        long int     id;                        /* Event identifier */
        double  when;                           /* Event time */

        {
        struct event_notice     *old_event, *new_event, *predecessor;

        new_event = (struct event_notice *) malloc(sizeof(struct event_notice));
        new_event -> event_time = when;
        new_event -> event_routine = routine;
        new_event -> id_number = id;

        predecessor = &zeroeth_event;

        while ((old_event = predecessor -> next_event) != NULL) /* Search loop */
                {
                if (when < old_event -> event_time)
                        break;

                predecessor = old_event;
                }

        new_event -> next_event = old_event;
        new_event -> prev_event = predecessor;

        predecessor -> next_event = new_event;
        old_event -> prev_event = new_event;

        return;

        }       /* End of schedule(what, when) */

simulate()
        {
        struct event_notice     *current_event;

rescan: current_event = zeroeth_event.next_event;

        while (TRUE)
                {
                if (current_event -> event_time > time)
                        time = current_event -> event_time;

                switch ((*(current_event -> event_routine)) (current_event -> id_number))
                        {
case NORMAL:            current_event -> prev_event -> next_event = current_event -> next_event;
                        current_event -> next_event -> prev_event = current_event -> prev_event;

                        free(current_event);
                        goto rescan;

case BLOCKED:           current_event = current_event -> next_event;
                        continue;

case EXIT:              return;
                        }
                }
        }

arrival(id)     /* Arrival Event */

        long int     id;

        {
        PCREATE("Car", id);
        PLACEON(id, "ENTRY");

        schedule(&arrival, id + 1, time + rvexpo(2.2)); /* Next arrival */

        schedule(&start_wash, id, time + 0.25);         /* Car moves to entry */

        return NORMAL;
        }

start_wash(id)  /* Start Wash Event */

        long int     id;

        {
        struct event_notice     *e;

        if (washer_busy)
                return BLOCKED;
        else
                {
                washer_busy = TRUE;
                PLACEON(id, "CHAIN");
                schedule(&start_dry, id, time + 2.0);   /* Wash time elapses */
                }

        return NORMAL;
        }

start_dry(id)   /* Start Dry Event */

        long int     id;

        {
        washer_busy = FALSE;

        schedule(&exit_sys, id, time + 1.5);    /* Drying time elapses */

        return NORMAL;
        }

exit_sys(id)    /* Exit System Event */

        long int     id;

        {
        PLACEON(id, "EXIT");

        schedule(&outta_here, id, time + 0.25);

        return NORMAL;
        }

outta_here(id)  /* Exit System Event */

        long int     id;

        {
        PDESTROY(id);

        if (--termination_counter > 0)
                return NORMAL;
        else
                {
                fprintf(trace_file, "End\n");
                return EXIT;
                }
        }

no_next_event(id)       /* Error condition */

        long int     id;

        {
        printf("\nNO NEW EVENT IN THE SYSTEM!/n/n");
        exit(1);
        }

double rvexpo(mean)     /* Exponential interarrival times */

        double  mean;

        {
        return -log(uniform()) * mean;  /* Well-known inverse transform */
        }

double uniform()                        /* simplified GPSS/H uniform generator */
        {
        static long int seed = 99999;

        double          rand;

        long int        product;

        /* Note: Not all C compilers will process this code properly; use with caution */

        product = (seed * 742938285) & 0x7fffffff;      /* Fishman/Moore "best" multiplier */

        product += (long int) ((double) seed * 0.345957598183304071);   /* Add high 31 bits */

        if (product & 0x80000000)
                product += 0x80000001;

        seed = product;

        rand = product / 2147483648.0;

        return rand;
        }


