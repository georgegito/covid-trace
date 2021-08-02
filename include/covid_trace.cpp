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

    pthread_mutex_lock(recent_contacts_queue->mut);
    queueAdd(recent_contacts_queue, BTnearMe2(*_cur_t));
    pthread_mutex_unlock(recent_contacts_queue->mut);

    // search every SEARCH_TIME seconds
    while (1) {
        usleep(SEARCH_TIME * 1000000);

        if (*_cur_t > END_TIME) break;

        recent_contacts.push_back(BTnearMe(*_cur_t));

        pthread_mutex_lock(recent_contacts_queue->mut);
        queueAdd(recent_contacts_queue, BTnearMe2(*_cur_t));
        pthread_mutex_unlock(recent_contacts_queue->mut);
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

        if (!recent_contacts_queue->empty) {
            // pthread_mutex_lock(recent_contacts_queue->mut);

            if (*_cur_t - recent_contacts_queue->buf[recent_contacts_queue->head]->timestamp > DEL_TIME) {

                pthread_mutex_lock(recent_contacts_queue->mut);
                queueDel(recent_contacts_queue);
                pthread_mutex_unlock(recent_contacts_queue->mut);
            }

            // pthread_mutex_unlock(recent_contacts_queue->mut);
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

void cont_prt(queue *q)
{
    printf("\nContacts:\n\n");

    if (q->empty == 1) {

        printf("No contacts\n");
        return;
    }

    if (q->full == 1) {
        
        int _count = 0;
        for (int i = q->head; ; i++) {
            if (i == QUEUESIZE)
                i = 0;

            printf("MAC Address: %ld\tTimestamp: %lf\n", q->buf[i]->macaddress, q->buf[i]->timestamp);

            if (++_count == QUEUESIZE)
                break;
        }

        return;
    }


    if (q->head < q->tail) {
        for (int i = q->head; i < q->tail; i++) {
            printf("MAC Address: %ld\tTimestamp: %lf\n", q->buf[i]->macaddress, q->buf[i]->timestamp);
        }

        return;
    }

    if (q->head > q->tail) {
        for (int i = q->head; ; i++) {
            if (i == QUEUESIZE)
                i = 0;

            if (i == q->tail)
                break;

            printf("MAC Address: %ld\tTimestamp: %lf\n", q->buf[i]->macaddress, q->buf[i]->timestamp);
        }
        
        return;
    }
}