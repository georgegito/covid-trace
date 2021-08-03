/* -------------------------------------------------------------------------- */
/*                                  main.cpp                                  */
/* -------------------------------------------------------------------------- */

#include <stdio.h>
#include <sys/time.h>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <headers.hpp>
#include <covid_trace.cpp>
#include <queue.cpp>

int main()
{
    /* ---------------------------- initialize rand() --------------------------- */
    time_t t;
    srand((unsigned) time(&t));

    /* ---------------------------- initialize timer ---------------------------- */
    double t0 = -1;
    double cur_t = -1;
    
    struct timeval tv_main;
    gettimeofday(&tv_main, NULL);
    t0 = tv_main.tv_sec * 1e6;
    t0 = (t0 + tv_main.tv_usec) * 1e-6;

    /* ------------------------ initialize contact queues ----------------------- */
    queue *recent_contacts_queue;
    queue *close_contacts_queue;
    recent_contacts_queue = queueInit();
    close_contacts_queue = queueInit();

    /* --------------------------- create output file --------------------------- */
    FILE *fptr;

    /* ------------------------ intialize argument struct ----------------------- */
    struct arg_struct *args = (struct arg_struct *)malloc(sizeof(struct arg_struct));
    args->arg1 = &t0;
    args->arg2 = &cur_t;
    args->arg3 = recent_contacts_queue;
    args->arg4 = close_contacts_queue;
    args->arg5 = fptr;

    /* ----------------------------- create threads ----------------------------- */
    pthread_t timer_thread;
    pthread_t test_thread;
    pthread_t rec_cont_thread;
    pthread_t cl_cont_thread;
    int rc;

    rc = pthread_create(&timer_thread, NULL, timer, (void *)args);
    if (rc) {
        printf("Error: return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    rc = pthread_create(&test_thread, NULL, test, (void *)args);
    if (rc) {
        printf("Error: return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    rc = pthread_create(&rec_cont_thread, NULL, rec_cont, (void *)args);
    // usleep(100000);
    if (rc) {
        printf("Error: return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    rc = pthread_create(&cl_cont_thread, NULL, cl_cont, (void *)args);
    if (rc) {
        printf("Error: return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    /* ------------------------------ sync threads ------------------------------ */
    pthread_join(timer_thread, NULL);
    pthread_join(test_thread, NULL);
    pthread_join(rec_cont_thread, NULL);
    pthread_join(cl_cont_thread, NULL);

    /* ----------------------------- print contacts ----------------------------- */
    printf("\nrecent_contacts_queue:\n\n");
    cont_prt(recent_contacts_queue);

    printf("\nclose_contacts_queue:\n\n");
    cont_prt(close_contacts_queue);

    /* ------------------------------- free memory ------------------------------ */
    for (int i = 0; i < QUEUESIZE; i ++)
        free(recent_contacts_queue->buf[i]);

    for (int i = 0; i < QUEUESIZE; i ++)
        free(close_contacts_queue->buf[i]);
    
    queueDelete(recent_contacts_queue);
    queueDelete(close_contacts_queue);

    return 0;
}