#include <stdio.h>
// #include <stdint.h>
// #include <stdbool.h>
#include <sys/time.h>
// #include <stdlib.h>
#include <vector>
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
    
    /* ------------------------ intialize argument struct ----------------------- */
    // struct arg_struct args;
    struct arg_struct *args = (struct arg_struct *)malloc(sizeof(struct arg_struct));
    args->arg1 = &t0;
    args->arg2 = &cur_t;

    /* ------------------------ initialize contact queues ----------------------- */
    recent_contacts_queue = queueInit();
    close_contacts_queue = queueInit();

    /* ----------------------------- create threads ----------------------------- */
    pthread_t timer_thread;
    pthread_t search_thread;
    pthread_t test_thread;
    pthread_t del_thread;
    pthread_t cl_cont_thread;
    int rc;

    rc = pthread_create(&timer_thread, NULL, timer, (void *)args);
    if (rc) {
        printf("Error: return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    rc = pthread_create(&search_thread, NULL, search, (void *)args);
    // usleep(100000);
    if (rc) {
        printf("Error: return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    rc = pthread_create(&test_thread, NULL, test, (void *)args);
    if (rc) {
        printf("Error: return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    rc = pthread_create(&del_thread, NULL, del, (void *)args);
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
    pthread_join(search_thread, NULL);
    pthread_join(test_thread, NULL);
    pthread_join(del_thread, NULL);
    pthread_join(cl_cont_thread, NULL);

    /* ------------------------------ print vectors ----------------------------- */
    std::cout << "\nrecent_contacts:\n\n";
    for (int i = 0; i < recent_contacts.size(); i++) {
        std::cout << "MAC Address: " << recent_contacts[i].macaddress << "\t" << "Timestamp: " << recent_contacts[i].timestamp << std::endl;
    }

    std::cout << "\nclose_contacts:\n\n";
    for (int i = 0; i < close_contacts.size(); i++) {
        std::cout << "MAC Address: " << close_contacts[i].macaddress << "\t" << "Timestamp: " << close_contacts[i].timestamp << std::endl;
    }

    printf("\nrecent_contacts_queue:\n\n");
    cont_prt(recent_contacts_queue);

    printf("\nclose_contacts_queue:\n\n");
    cont_prt(close_contacts_queue);

    /* ------------------------------- free memory ------------------------------ */
    queueDelete(recent_contacts_queue);
    queueDelete(close_contacts_queue);

    // TODO free malloc

    return 0;
}