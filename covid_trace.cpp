#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/time.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <pthread.h>

#define SEARCH_TIME 2 // seconds
#define TEST_TIME 2 // seconds
#define DEL_TIME 20 // seconds
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

void *cl_cont(void *arg)
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

                        contact _close_contact;
                        _close_contact.macaddress = recent_contacts[i].macaddress;
                        _close_contact.timestamp = mean_timestamp;

                        // delete duplicate close contacts
                        for (int k = 0; k < close_contacts.size(); k++) {
                            if (_close_contact.macaddress == close_contacts[k].macaddress) {
                                close_contacts.erase(close_contacts.begin() + k);
                            }
                        }

                        // add new close contact
                        close_contacts.push_back(_close_contact);
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