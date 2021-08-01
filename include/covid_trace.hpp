#ifndef __COVID_TRACE_HPP__
#define __COVID_TRACE_HPP__

#define SEARCH_TIME 2 // seconds
#define TEST_TIME 2 // seconds
#define DEL_TIME 20 // seconds
#define NUM_OF_ADDRESSES 3
#define MIN_CLOSE_CONTACT_TIME 1 // seconds
#define MAX_CLOSE_CONTACT_TIME 5 // seconds
#define POS_TEST_PROP 25 // %, must divide 100
#define END_TIME 15 // seconds
#define CLOSE_DEL_TIME 20 // seconds

typedef struct contact
{
    uint64_t macaddress : 48;
    double timestamp;
} contact;

struct arg_struct
{
    double *arg1;
    double *arg2;
};

/* ----------------------- initialize global variables ---------------------- */
std::vector<contact> recent_contacts;
std::vector<contact> close_contacts;
// double t0 = -1;
// double cur_t = -1;
FILE *fptr;

/* -------------------------------------------------------------------------- */

contact BTnearMe(double timestamp)
{
    contact _contact;
    _contact.macaddress = rand() % NUM_OF_ADDRESSES;
    _contact.timestamp = timestamp;

    return _contact;
}

bool testCOVID() // positive result propability: POS_TEST_PROP %
{
    int temp1 = 100 / POS_TEST_PROP;
    int temp2 = rand() % temp1;
    
    if (temp2 == 0) {
        std::cout << "Positive COVID test\n";
        return true;
    }
    else {
        // std::cout << "Negative COVID test\n";
        return false;
    }
}

void uploadContacts(double cur_t)
{
    // fptr = fopen("close_contacts.bin","ab+");
    fptr = fopen("close_contacts.txt","a+");

    if(fptr == NULL)
    {
        printf("Error!");   
        exit(1);             
    }

    // write close contacts to file
    for (int i = 0; i < close_contacts.size(); i++) {
        fprintf(fptr, "%ld\t%lf\t%lf\n", close_contacts[i].macaddress, close_contacts[i].timestamp, cur_t);
        // fwrite(&close_contacts[i], sizeof(contact), 1, fptr);
        // fwrite(&close_contacts[i].timestamp, sizeof(double), 1, fptr);
    }

    fclose(fptr);

    return;
}

void *timer(void *arg)
{
    struct arg_struct *args = (struct arg_struct*)arg;
    double *_t0 = args->arg1;
    double *_cur_t = args->arg2;

    // initialize timer
    struct timeval tv_timer;
    double timer_t;

    while (1) {
        gettimeofday(&tv_timer, NULL);
        timer_t = tv_timer.tv_sec * 1e6;
        timer_t = (timer_t + tv_timer.tv_usec) * 1e-6;

        *_cur_t = timer_t - *_t0;

        if (*_cur_t > END_TIME) break;

        usleep(100); // TODO check efficiency
    }

    return (NULL);
}

void *search(void *arg)
{
    struct arg_struct *args = (struct arg_struct*)arg;
    double *_t0 = args->arg1;
    double *_cur_t = args->arg2;

    // first search
    recent_contacts.push_back(BTnearMe(*_cur_t));

    // search every SEARCH_TIME seconds
    while (1) {
        usleep(SEARCH_TIME * 1000000);

        if (*_cur_t > END_TIME) break;

        recent_contacts.push_back(BTnearMe(*_cur_t));
        // std::cout << "I am a test thread with id = " << id << " and time = " << cur_t << std::endl;
    }

    return (NULL);
}

void *test(void *arg)
{
    struct arg_struct *args = (struct arg_struct*)arg;
    double *_t0 = args->arg1;
    double *_cur_t = args->arg2;

    // fptr = fopen("close_contacts.bin","wb");
    fptr = fopen("close_contacts.txt","w");
    fclose(fptr);

    // first test
    if (testCOVID()) 
        uploadContacts(*_cur_t);

    // test every TEST_TIME seconds
    while (1) {
        usleep(TEST_TIME * 1000000);

        if (*_cur_t > END_TIME) break;

        if (testCOVID())
            uploadContacts(*_cur_t);

        std::cout << "Test time: " << *_cur_t << std::endl;        
    }

    return (NULL);
}

void *del(void *arg)
{
    struct arg_struct *args = (struct arg_struct*)arg;
    double *_t0 = args->arg1;
    double *_cur_t = args->arg2;

    while (1) {
        usleep(500000);

        if (*_cur_t > END_TIME) break;
        
        if (recent_contacts.size() > 0) {
            if (*_cur_t - recent_contacts[0].timestamp > DEL_TIME)
                recent_contacts.erase(recent_contacts.begin()); // TODO change data structure & use mutex
        } 
        
        // std::cout << "Current time: " << cur_t << std::endl;
    }

    return (NULL);
}

void *cl_cont(void *arg)
{
    struct arg_struct *args = (struct arg_struct*)arg;
    double *_t0 = args->arg1;
    double *_cur_t = args->arg2;

    while (1) {
        usleep(500000);

        if (*_cur_t > END_TIME) break;

        if (close_contacts.size() > 0) {
            if (*_cur_t - close_contacts[0].timestamp > CLOSE_DEL_TIME)
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

#endif // COVID_TRACE_HPP