#include <stdio.h>
// #include <stdint.h>
// #include <stdbool.h>
#include <sys/time.h>
// #include <stdlib.h>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <covid_trace.hpp>

int main()
{
    /* ---------------------------- initialize rand() --------------------------- */
    time_t t;
    srand((unsigned) time(&t));

    /* ---------------------------- initialize timer ---------------------------- */
    struct timeval tv_main;
    gettimeofday(&tv_main, NULL);
    t0 = tv_main.tv_sec * 1e6;
    t0 = (t0 + tv_main.tv_usec) * 1e-6;

    /* ----------------------------- create threads ----------------------------- */
    pthread_t timer_thread;
    pthread_t search_thread;
    pthread_t test_thread;
    pthread_t del_thread;
    pthread_t cl_cont_thread;
    int rc;

    rc = pthread_create(&timer_thread, NULL, timer, NULL);
    if (rc) {
        printf("Error: return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    rc = pthread_create(&search_thread, NULL, search, NULL);
    // usleep(100000);
    if (rc) {
        printf("Error: return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    rc = pthread_create(&test_thread, NULL, test, NULL);
    if (rc) {
        printf("Error: return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    rc = pthread_create(&del_thread, NULL, del, NULL);
    if (rc) {
        printf("Error: return code from pthread_create() is %d\n", rc);
        exit(-1);
    }

    rc = pthread_create(&cl_cont_thread, NULL, cl_cont, NULL);
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
    std::cout << "\nrecent_contacts\n\n";
    for (int i = 0; i < recent_contacts.size(); i++) {
        std::cout << "MAC Address: " << recent_contacts[i].macaddress << "\t" << "Timestamp: " << recent_contacts[i].timestamp << std::endl;
    }

    std::cout << "\nclose_contacts\n\n";
    for (int i = 0; i < close_contacts.size(); i++) {
        std::cout << "MAC Address: " << close_contacts[i].macaddress << "\t" << "Timestamp: " << close_contacts[i].timestamp << std::endl;
    }

    return 0;
}