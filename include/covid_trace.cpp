typedef struct cont
{
    contact var1;
    contact *var2;
} cont;

contact BTnearMe(double timestamp)
{
    contact _contact;
    _contact.macaddress = rand() % NUM_OF_ADDRESSES;
    _contact.timestamp = timestamp;

    return _contact;
}

contact *BTnearMe2(double timestamp)
{
    contact *_contact = (contact *)malloc(sizeof(contact));
    _contact->macaddress = rand() % NUM_OF_ADDRESSES;
    _contact->timestamp = timestamp;

    return _contact;
}

cont* BTnearMe3(double timestamp)
{
    cont* _cont = (cont *)malloc(sizeof(cont));
    _cont->var2 = (contact *)malloc(sizeof(contact));

    _cont->var1.macaddress = rand() % NUM_OF_ADDRESSES;
    _cont->var2->macaddress = _cont->var1.macaddress;
    _cont->var1.timestamp = timestamp;
    _cont->var2->timestamp = timestamp;

    return _cont;
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
    // // fptr = fopen("close_contacts.bin","ab+");
    // fptr = fopen("close_contacts.txt","a+");

    // if(fptr == NULL)
    // {
    //     printf("Error!");   
    //     exit(1);             
    // }

    // // write close contacts to file
    // for (int i = 0; i < close_contacts.size(); i++) {
    //     fprintf(fptr, "%ld\t%lf\t%lf\n", close_contacts[i].macaddress, close_contacts[i].timestamp, cur_t);
    //     // fwrite(&close_contacts[i], sizeof(contact), 1, fptr);
    //     // fwrite(&close_contacts[i].timestamp, sizeof(double), 1, fptr);
    // }

    // fclose(fptr);

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
    cont *temp_cont = (cont *)malloc(sizeof(cont)); 
    
    temp_cont = BTnearMe3(*_cur_t);

    recent_contacts.push_back(temp_cont->var1);

    pthread_mutex_lock(recent_contacts_queue->mut);
    queueAdd(recent_contacts_queue, temp_cont->var2);
    pthread_mutex_unlock(recent_contacts_queue->mut);

/* -------------------------------------------------------------------------- */

    // recent_contacts.push_back(BTnearMe(*_cur_t));

    // pthread_mutex_lock(recent_contacts_queue->mut);
    // queueAdd(recent_contacts_queue, BTnearMe2(*_cur_t));
    // pthread_mutex_unlock(recent_contacts_queue->mut);

/* -------------------------------------------------------------------------- */


    // search every SEARCH_TIME seconds
    while (1) {
        usleep(SEARCH_TIME * 1000000);

        if (*_cur_t > END_TIME) break;

        temp_cont = BTnearMe3(*_cur_t);

        recent_contacts.push_back(temp_cont->var1);

        pthread_mutex_lock(recent_contacts_queue->mut);
        queueAdd(recent_contacts_queue, temp_cont->var2);
        pthread_mutex_unlock(recent_contacts_queue->mut);

/* -------------------------------------------------------------------------- */

        // recent_contacts.push_back(BTnearMe(*_cur_t));

        // pthread_mutex_lock(recent_contacts_queue->mut);
        // queueAdd(recent_contacts_queue, BTnearMe2(*_cur_t));
        // pthread_mutex_unlock(recent_contacts_queue->mut);

/* -------------------------------------------------------------------------- */
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

/* ---------------------- vector way - will be deleted ---------------------- */

        if (recent_contacts.size() > 0) {
            if (*_cur_t - recent_contacts[0].timestamp > DEL_TIME)
                recent_contacts.erase(recent_contacts.begin()); // TODO change data structure & use mutex
        }

/* ---------------------------- static queue way ---------------------------- */

        if (!recent_contacts_queue->empty) {

            if (*_cur_t - recent_contacts_queue->buf[recent_contacts_queue->head]->timestamp > DEL_TIME) {

                pthread_mutex_lock(recent_contacts_queue->mut);
                queueDel(recent_contacts_queue);
                pthread_mutex_unlock(recent_contacts_queue->mut);
            }
        }

/* -------------------------------------------------------------------------- */
    }

    return (NULL);
}

void *cl_cont(void *arg)
{
    struct arg_struct *args = (struct arg_struct*)arg;
    double *_t0 = args->arg1;
    double *_cur_t = args->arg2;

//     while (1) {
//         usleep(500000);

//         if (*_cur_t > END_TIME) break;

// /* ---------------------- vector way - will be deleted ---------------------- */

//         if (close_contacts.size() > 0) {
//             if (*_cur_t - close_contacts[0].timestamp > CLOSE_DEL_TIME)
//                 close_contacts.erase(close_contacts.begin()); // TODO change data structure & use mutex
//         }

// /* ---------------------------- static queue way ---------------------------- */
// // TODO
//         // if (!close_contacts_queue->empty) {

//         //     int i = close_contacts_queue->head;
//         //     do {
//         //         // std::cout << "i = " << i << "\tclose_contacts_queue->head = " << close_contacts_queue->head << "\tclose_contacts_queue->tail = " << close_contacts_queue->tail << std::endl;
//         //             if (i == QUEUESIZE) {
//         //                 i = 0; 
//         //                 if (i == close_contacts_queue->tail)
//         //                     break;
//         //             }

//         //             if (*_cur_t - close_contacts_queue->buf[i]->timestamp > CLOSE_DEL_TIME && close_contacts_queue->buf[i]->macaddress != 99) {
//         //                 close_contacts_queue->buf[i]->macaddress = 99;
//         //                 // std::cout << close_contacts_queue->buf[i]->timestamp << std::endl;
//         //             }
//         //                 // TODO queue_erase

//         //         i++;
//         //     } while (i != close_contacts_queue->tail);

//         //     // if (*_cur_t - close_contacts_queue->buf[recent_contacts_queue->head]->timestamp > CLOSE_DEL_TIME) {
//         //     //     // pthread_mutex_lock(close_contacts_queue->mut); // mutex is not needed
//         //     //     queueDel(close_contacts_queue);
//         //     //     // pthread_mutex_unlock(close_contacts_queue->mut);
//         //     // }
//         // }

// /* ---------------------- vector way - will be deleted ---------------------- */

//         // for (int i = 0; i < recent_contacts.size() - 1; i++) {
//         //     for (int j = i + 1; j < recent_contacts.size(); j++) {

//         //         if (recent_contacts[i].macaddress == recent_contacts[j]. macaddress) {

//         //             double dt = recent_contacts[j].timestamp - recent_contacts[i].timestamp;
                    
//         //             if (dt < 0) {
//         //                 std::cout << "ERROR\n";
//         //                 return (NULL);
//         //             }
                    
//         //             if (dt <= MAX_CLOSE_CONTACT_TIME && dt >= MIN_CLOSE_CONTACT_TIME) {
                        
//         //                 std::cout << "Hi Git 1\n";
                        
//         //                 double mean_timestamp = (recent_contacts[i].timestamp + recent_contacts[j].timestamp) / 2;

//         //                 contact _close_contact;
//         //                 _close_contact.macaddress = recent_contacts[i].macaddress;
//         //                 _close_contact.timestamp = mean_timestamp;

//         //                 // delete duplicate close contacts
//         //                 for (int k = 0; k < close_contacts.size(); k++) {
//         //                     if (_close_contact.macaddress == close_contacts[k].macaddress) {
//         //                         close_contacts.erase(close_contacts.begin() + k);
//         //                     }
//         //                 }

//         //                 // add new close contact
//         //                 close_contacts.push_back(_close_contact);
//         //             }
//         //         }
//         //     }
//         // }

// /* ---------------------------- static queue way ---------------------------- */

//         bool broken = false;
//         if (!recent_contacts_queue->empty) {
//             for (int i = recent_contacts_queue->head; ; i++) {

//                 if (i == QUEUESIZE)
//                     i = 0;

//                 for (int j = i + 1; ; j++) {

//                     if (j == QUEUESIZE)
//                         j = 0;

//                     if (j == recent_contacts_queue->tail) {

//                         if (i == j - 1 || (j == 0 && i == QUEUESIZE - 1))
//                             broken = true;
//                         break;
//                     }

//                     if (recent_contacts_queue->buf[i]->macaddress == recent_contacts_queue->buf[j]->macaddress) {

//                         double dt = recent_contacts_queue->buf[j]->timestamp - recent_contacts_queue->buf[i]->timestamp;

//                         if (dt < 0) {
//                             std::cout << "ERROR\n";
//                             return (NULL);
//                         }

//                         if (dt <= MAX_CLOSE_CONTACT_TIME && dt >= MIN_CLOSE_CONTACT_TIME) {

//                             double mean_timestamp = (recent_contacts_queue->buf[i]->timestamp + recent_contacts_queue->buf[j]->timestamp) / 2; 
                            
//                             contact *_close_contact = (contact *)malloc(sizeof(contact));
//                             _close_contact->macaddress = recent_contacts_queue->buf[i]->macaddress;
//                             _close_contact->timestamp = mean_timestamp;

//                             // avoid duplicate close contacts by replacing timestamp in existing close contacts
//                             bool added = false;
//                             if (!close_contacts_queue->empty) {

//                                 int k = close_contacts_queue->head;
//                                 do {
//                                         if (k == QUEUESIZE) {
//                                             k = 0;
//                                             if (k == close_contacts_queue->tail)
//                                                 break;
//                                         }

//                                         if (_close_contact->macaddress == close_contacts_queue->buf[k]->macaddress) {
//                                             close_contacts_queue->buf[k]->timestamp = mean_timestamp; // TODO
//                                             added = true;
//                                             // std::cout << "Existing close contact timestamp renewed\n";
//                                         }
//                                         k++;
//                                 } while (k != close_contacts_queue->tail);
//                             }
//                             // add new close contact if it does not already exist
//                             if (!added) {
//                                 // pthread_mutex_lock(close_contacts_queue->mut); // mutex is not needed
//                                 queueAdd(close_contacts_queue, _close_contact);
//                                 // pthread_mutex_unlock(close_contacts_queue->mut);
//                                 // std::cout << "Close contact added\n";
//                             }
//                         }
//                     }
//                 }
//                 if (broken)
//                     break;
//             }
//         }


// /* -------------------------------------------------------------------------- */
//     }

    return (NULL);
}

void cont_prt(queue *q)
{
    if (q->empty == 1) {

        printf("No contacts\n");
        return;
    }

    int i = q->head;
    do {
            if (i == QUEUESIZE) {
                i = 0;
                if (i == q->tail)
                    break;
            }

            printf("MAC Address: %ld\tTimestamp: %lf\n", q->buf[i]->macaddress, q->buf[i]->timestamp);
            i++;
    } while (i != q->tail);
}