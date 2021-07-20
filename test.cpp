#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <math.h>
#include <unistd.h>
#include <pthread.h>

#define SEARCH_TIME 2 // seconds
#define TEST_TIME 2 // seconds
#define MAX_TIME 10 // seconds
#define DEL_TIME 4 // seconds
#define NUM_OF_ADDRESSES 5;
#define MIN_CLOSE_CONTACT_TIME 1 // seconds
#define MAX_CLOSE_CONTACT_TIME 5 // seconds
#define NUM_OF_THREADS 1
#define POS_TEST_PROP 10 // %, must divide 100
#define END_TIME 15 // seconds
#define CLOSE_DEL_TIME 20 // seconds

typedef struct contact
{
    uint64_t macaddress : 48;
    double timestamp;
} contact;

/* ----------------------- initialize global variables ---------------------- */
std::vector<contact> recent_contacts;
std::vector<contact> close_contacts;
double t0 = -1;
double cur_t = -1;
/* -------------------------------------------------------------------------- */

contact BTnearMe(double timestamp)
{
    contact _contact;
    _contact.macaddress = rand() % NUM_OF_ADDRESSES;
    _contact.timestamp = timestamp;

    return _contact;
}

bool testCOVID() // positive result propability: 5%
{
    int temp1 = 100 / POS_TEST_PROP;
    int temp2 = rand() % temp1;
    
    if (temp2 == 0) {
        std::cout << "Positive COVID test\n";
        return true;
    }
    else {
        std::cout << "Negative COVID test\n";
        return false;
    }
}

void uploadContacts(contact* arg1, int arg2)
{
    return;
}

void *timer(void *arg)
{
    // initialize timer
    struct timeval tv_timer;
    double timer_t;

    while(1) {
        gettimeofday(&tv_timer, NULL);
        timer_t = tv_timer.tv_sec * 1e6;
        timer_t = (timer_t + tv_timer.tv_usec) * 1e-6;

        cur_t = timer_t - t0;

        if (cur_t > END_TIME) break;

        usleep(100); // TODO check efficiency
    }

    return(NULL);
}

void *search(void *arg)
{
    // int id = *(int *)arg;

    // first search
    recent_contacts.push_back(BTnearMe(cur_t));

    // search every SEARCH_TIME seconds
    while(1) {
        usleep(SEARCH_TIME * 1000000);

        if (cur_t > END_TIME) break;

        recent_contacts.push_back(BTnearMe(cur_t));
        // std::cout << "I am a test thread with id = " << id << " and time = " << cur_t << std::endl;
    }

  return (NULL);
}

void *test(void *arg)
{
    // first test
    testCOVID();

    // test every TEST_TIME seconds
    while(1) {
        usleep(TEST_TIME * 1000000);

        if (cur_t > END_TIME) break;

        testCOVID();

        std::cout << "Test time: " << cur_t << std::endl;        
    }

  return (NULL);
}

void *del(void *arg)
{
    while(1) {
        usleep(500000);

        if (cur_t > END_TIME) break;
        
        if (recent_contacts.size() > 0) {
            if (cur_t - recent_contacts[0].timestamp > DEL_TIME)
                recent_contacts.erase(recent_contacts.begin()); // TODO change data structure & use mutex
        } 
        
        // std::cout << "Current time: " << cur_t << std::endl;
    }

    return (NULL);
}

void *find_close_contacts(void *arg)
{
    while(1) {
        usleep(500000);

        if (cur_t > END_TIME) break;

        if (close_contacts.size() > 0) {
            if (cur_t - close_contacts[0].timestamp > CLOSE_DEL_TIME)
                close_contacts.erase(close_contacts.begin()); // TODO change data structure & use mutex
        }

        for (int i = 0; i < recent_contacts.size() - 1; i++) {
            for (int j = i + 1; j < recent_contacts.size(); j++) {

                if (recent_contacts[i].macaddress == recent_contacts[j]. macaddress) {

                    int dt = recent_contacts[j].timestamp - recent_contacts[i].timestamp;
                    
                    if (dt < 0) {
                        std::cout << "ERROR\n";
                        return (NULL);
                    }
                    
                    if (dt <= MAX_CLOSE_CONTACT_TIME && dt >= MIN_CLOSE_CONTACT_TIME) {
                        double mean_timestamp = (recent_contacts[i].timestamp + recent_contacts[j].timestamp) / 2;
                        // TODO
                    }
                }
            }
        }
    }

    return (NULL);
}

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
    // pthread_t *test_threads = (pthread_t *)malloc(sizeof(pthread_t) * NUM_OF_THREADS);
    pthread_t timer_thread;
    pthread_t search_thread;
    pthread_t test_thread;
    pthread_t del_thread;
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

    // /* -------------------------------------------------------------------------- */
    // /*             t0: time zero of the system, t1: last search time,             */
    // /*                    t2: current time, t3: last test time                    */
    // /* -------------------------------------------------------------------------- */
    // double t1 = t0;
    // double t2 = t0;
    // double t3 = t0;

    // // first search
    // recent_contacts.push_back(BTnearMe(t2 - t0));

    // // first test
    // testCOVID();

    // // FILE* f = fopen("data.txt", "w+");

    // /* -------------------------------- main loop ------------------------------- */
    // while (1) {
    //     // record current moment t2
    //     gettimeofday(&tv, NULL);
    //     t2 = tv.tv_sec * 1e6;
    //     t2 = (t2 + tv.tv_usec) * 1e-6;

    //     // compute the difference between current moment and last search
    //     double search_dt = t2 - t1;

    //     // if the difference is >= than the search time, execute a new search
    //     if (search_dt >= SEARCH_TIME) {

    //         // add the new contact to recent_contacts vector
    //         recent_contacts.push_back(BTnearMe(t2 - t0));

    //         // fprintf(f, "MAC Address: %d\t Timestamp: %lf\n", _macaddress, _timestamp);
    //         // printf("%lf\n", t2 - t0);

    //         // record the last search moment
    //         t1 = t2;

    //         /* -------------------- delete contacts after delete time ------------------- */
    //         for (int i = 0; i < recent_contacts.size(); i++) {
    //             if ((t2 - t0) - recent_contacts[i].timestamp > DEL_TIME)
    //                 recent_contacts.erase(recent_contacts.begin() + i);
    //         }

    //         /* ------------------------ check for close contacts ------------------------ */

    //         for (int i = 0; i < recent_contacts.size() - 1; i++) {
    //             for (int j = i + 1; j < recent_contacts.size(); j++) {
    //                 if (recent_contacts[i].macaddress == recent_contacts[j].macaddress
    //                     && abs(recent_contacts[i].timestamp - recent_contacts[j].timestamp) <= MAX_CLOSE_CONTACT_TIME
    //                     && abs(recent_contacts[i].timestamp - recent_contacts[j].timestamp) >= MIN_CLOSE_CONTACT_TIME) {
    //                     contact _contact;
    //                     _contact.macaddress = recent_contacts[i].macaddress;
    //                     _contact.timestamp = (recent_contacts[i].timestamp + recent_contacts[j].timestamp) / 2;
    //                     std::cout << _contact.macaddress << std::endl;
    //                     close_contacts.push_back(_contact);
    //                 }
    //             }
    //         }
    //     }

    //     // compute the difference between current moment and last test
    //     double test_dt = t2 - t3;
        
    //     // if the difference is >= than the test time, execute a new test
    //     if (test_dt >= TEST_TIME) {

    //         // execute test
    //         testCOVID();

    //         // record the last test moment
    //         t3 = t2;
    //     }

    //     /* ----------------------------- terminate loop ----------------------------- */
    //     if (t2 - t0 > MAX_TIME) break;
    // }

    /* ------------------------------ sync threads ------------------------------ */
    pthread_join(timer_thread, NULL);    
    pthread_join(search_thread, NULL);
    pthread_join(test_thread, NULL);
    pthread_join(del_thread, NULL);

    /* ------------------------------ print vectors ----------------------------- */
    std::cout << "\nrecent_contacts\n\n";
    for (int i = 0; i < recent_contacts.size(); i++) {
        std::cout << "MAC Address: " << recent_contacts[i].macaddress << "\t" << "Timestamp: " << recent_contacts[i].timestamp << std::endl;
    }

    std::cout << "\nclose_contacts\n\n";
    for (int i = 0; i < close_contacts.size(); i++) {
        std::cout << "MAC Address: " << close_contacts[i].macaddress << "\t" << "Timestamp: " << close_contacts[i].timestamp << std::endl;
    }

    // fclose(f);

    return 0;
}