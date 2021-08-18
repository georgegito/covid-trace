/* -------------------------------------------------------------------------- */
/*                                  headers.h                                 */
/* -------------------------------------------------------------------------- */

#define SEARCH_TIME 10 // seconds
#define TEST_TIME 4 * 60 * 60 / 100 // seconds
#define RECENT_DEL_TIME 20 * 60 / 100 // seconds
#define NUM_OF_ADDRESSES 30
#define MIN_CLOSE_CONTACT_TIME 4 * 60 / 100 // seconds
#define MAX_CLOSE_CONTACT_TIME 20 * 60 / 100 // seconds
#define POS_TEST_PROP 10 // %, must divide 100
#define END_TIME 30 * 24 * 60 * 60 / 100 // seconds
#define CLOSE_DEL_TIME 14 * 24 * 60 * 60 / 100 // seconds
#define RECENT_QUEUESIZE (RECENT_DEL_TIME / SEARCH_TIME + 1)
#define CLOSE_QUEUESIZE ((CLOSE_DEL_TIME / SEARCH_TIME) * ((MAX_CLOSE_CONTACT_TIME - MIN_CLOSE_CONTACT_TIME) / SEARCH_TIME) + 1)

/* --------------------------------- structs -------------------------------- */
typedef struct
{
  unsigned long long macaddress : 48;
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
  FILE** arg5;
};

/* ---------------------------- declare functions --------------------------- */
queue* queue_init(int bufSize);
void queue_delete(queue* q);
void queue_add(queue* q, contact* in);
void queue_del(queue* q);

contact* bt_near_me(double timestamp, FILE* fptr);
bool test_covid();
void upload_contacts(double cur_t, FILE** fptr, queue* close_contacts_queue);
void* timer(void* arg);
void* test(void* arg);
void* rec_cont(void* arg);
void* cl_cont(void* arg);
void cont_prt(queue* q);
int bin_file_size(const char* filename);
void read_cont_bin();
void read_bt_search_times_bin();