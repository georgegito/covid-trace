/* -------------------------------------------------------------------------- */
/*                                  headers.h                                 */
/* -------------------------------------------------------------------------- */

#ifndef __HEADERS_HPP__
#define __HEADERS_HPP__

#define SEARCH_TIME 2 // seconds
#define TEST_TIME 2 // seconds
#define RECENT_DEL_TIME 20 // seconds
#define NUM_OF_ADDRESSES 4
#define MIN_CLOSE_CONTACT_TIME 1 // seconds
#define MAX_CLOSE_CONTACT_TIME 6 // seconds
#define POS_TEST_PROP 50 // %, must divide 100
#define END_TIME 15 // seconds
#define CLOSE_DEL_TIME 20 // seconds
#define RECENT_QUEUESIZE (RECENT_DEL_TIME / SEARCH_TIME + 1)
#define CLOSE_QUEUESIZE ((CLOSE_DEL_TIME / SEARCH_TIME) * ((MAX_CLOSE_CONTACT_TIME - MIN_CLOSE_CONTACT_TIME) / SEARCH_TIME) + 1) // TODO check

/* --------------------------------- structs -------------------------------- */
typedef struct contact
{
  unsigned long macaddress : 48;
  double timestamp;
} contact;

typedef struct
{
  contact** buf;
  long head, tail;
  int full, empty, lastAddIndex, bufSize;
  pthread_mutex_t* mut;
  pthread_cond_t* notFull, * notEmpty;
} queue;

struct arg_struct
{
  double* arg1;
  double* arg2;
  queue* arg3;
  queue* arg4;
  FILE* arg5;
};

/* ---------------------------- declare functions --------------------------- */
queue* queueInit(int bufSize);
void queueDelete(queue* q);
void queueAdd(queue* q, contact* in);
void queueDel(queue* q);

contact* BTnearMe(double timestamp);
bool testCOVID();
void uploadContacts(double cur_t, FILE* fptr, queue* close_contacts_queue);
void* timer(void* arg);
void* test(void* arg);
void* rec_cont(void* arg);
void* cl_cont(void* arg);
void cont_prt(queue* q);
int bin_file_size(const char* filename);
void read_bin(const char* filename);

/* -------------------------------------------------------------------------- */

#endif // __HEADERS_HPP__
