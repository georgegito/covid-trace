/* -------------------------------------------------------------------------- */
/*                                 headers.hpp                                */
/* -------------------------------------------------------------------------- */

#ifndef __HEADERS_HPP__
#define __HEADERS_HPP__

#define SEARCH_TIME 2 // seconds
#define TEST_TIME 2 // seconds
#define DEL_TIME 30 // seconds
#define NUM_OF_ADDRESSES 3
#define MIN_CLOSE_CONTACT_TIME 1 // seconds
#define MAX_CLOSE_CONTACT_TIME 5 // seconds
#define POS_TEST_PROP 25 // %, must divide 100
#define END_TIME 15 // seconds
#define CLOSE_DEL_TIME 20 // seconds
#define QUEUESIZE (DEL_TIME / SEARCH_TIME + 1)

typedef struct contact
{
    uint64_t macaddress : 48;
    double timestamp;
} contact;

typedef struct {
  contact *buf[QUEUESIZE];
  long head, tail;
  int full, empty, lastAddIndex;
  pthread_mutex_t *mut;
  pthread_cond_t *notFull, *notEmpty;
} queue;

struct arg_struct
{
    double *arg1;
    double *arg2;
    queue *arg3;
    queue *arg4;
    FILE *arg5;
};

/* ---------------------------- declare functions --------------------------- */
queue *queueInit();
void queueDelete(queue *q);
void queueAdd(queue *q, contact *in);
void queueDel(queue *q);

contact *BTnearMe(double timestamp);
bool testCOVID();
void uploadContacts(double cur_t);
void *timer(void *arg);
void *test(void *arg);
void *rec_cont(void *arg);
void *cl_cont(void *arg);
void cont_prt(queue *q);

/* -------------------------------------------------------------------------- */

#endif // __HEADERS_HPP__
