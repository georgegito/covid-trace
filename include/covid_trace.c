/* -------------------------------------------------------------------------- */
/*                                covid_trace.c                               */
/* -------------------------------------------------------------------------- */

contact* BTnearMe(double timestamp)
{
    contact* _contact = (contact*)malloc(sizeof(contact));
    _contact->macaddress = rand() % NUM_OF_ADDRESSES;
    _contact->timestamp = timestamp;

    return _contact;
}

bool testCOVID() // positive result propability: POS_TEST_PROP %
{
    int temp1 = 100 / POS_TEST_PROP;
    int temp2 = rand() % temp1;

    if (temp2 == 0) {
        // printf("Positive COVID test\n");
        return true;
    }
    else {
        // printf("Negative COVID test\n");
        return false;
    }
}

void uploadContacts(double cur_t, FILE* fptr, queue* close_contacts_queue)
{
    if (close_contacts_queue->empty == 1) {
        return;
    }

    // open file for appending binary
    fptr = fopen("close_contacts.bin", "ab");
    if (fptr == NULL) {
        printf("Error!");
        exit(1);
    }

    // // append upload time to the file
    // size_t elements_written = fwrite(&cur_t, sizeof(double), 1, fptr);
    // if (elements_written == 0) {
    //     printf("Error!");
    //     fclose(fptr);
    //     exit(1);
    // }

    // append close contacts to the file
    int i = close_contacts_queue->head;
    do {
        if (i == close_contacts_queue->bufSize) {
            i = 0;
            if (i == close_contacts_queue->tail)
                break;
        }

        size_t elements_written = fwrite(close_contacts_queue->buf[i], sizeof(contact), 1, fptr);
        if (elements_written == 0) {
            printf("Error!");
            fclose(fptr);
            exit(1);
        }

        i++;
    } while (i != close_contacts_queue->tail);

    fclose(fptr);

    return;
}

void* timer(void* arg)
{
    struct arg_struct* args = (struct arg_struct*)arg;
    double* _t0 = args->arg1;
    double* _cur_t = args->arg2;

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

void* test(void* arg)
{
    struct arg_struct* args = (struct arg_struct*)arg;
    double* _t0 = args->arg1;
    double* _cur_t = args->arg2;
    queue* _close_contacts_queue = args->arg4;
    FILE* _fptr = args->arg5;

    _fptr = fopen("close_contacts.bin", "wb");
    if (_fptr == NULL) {
        printf("Error!");
        exit(1);
    }
    // _fptr = fopen("close_contacts.txt", "w");
    fclose(_fptr);

    // first test
    if (testCOVID()) {
        printf("Positive test (time: %lf)\n", *_cur_t);
        uploadContacts(*_cur_t, _fptr, _close_contacts_queue);
    }

    // test every TEST_TIME seconds
    while (1) {
        usleep(TEST_TIME * 1000000);

        if (*_cur_t > END_TIME) break;

        if (testCOVID()) {
            printf("Positive test (time: %lf)\n", *_cur_t);
            uploadContacts(*_cur_t, _fptr, _close_contacts_queue);
        }
    }

    return (NULL);
}

void* rec_cont(void* arg) // TODO will be renamed to rec_cont
{
    struct arg_struct* args = (struct arg_struct*)arg;
    double* _t0 = args->arg1;
    double* _cur_t = args->arg2;
    queue* _recent_contacts_queue = args->arg3;

    // first add
    queueAdd(_recent_contacts_queue, BTnearMe(*_cur_t));

    // search every SEARCH_TIME seconds
    while (1) {
        usleep(SEARCH_TIME * 1000000);

        if (*_cur_t > END_TIME) break;

        /* -------------------------------------------------------------------------- */
        /*                                pop contacts                                */
        /* -------------------------------------------------------------------------- */

        if (!_recent_contacts_queue->empty) {

            if (*_cur_t - _recent_contacts_queue->buf[_recent_contacts_queue->head]->timestamp > RECENT_DEL_TIME) {

                // pthread_mutex_lock(recent_contacts_queue->mut);
                queueDel(_recent_contacts_queue);
                // pthread_mutex_unlock(recent_contacts_queue->mut);
            }
        }

        /* -------------------------------------------------------------------------- */
        /*                                add contacts                                */
        /* -------------------------------------------------------------------------- */

        queueAdd(_recent_contacts_queue, BTnearMe(*_cur_t));
    }

    return (NULL);
}

void* cl_cont(void* arg)
{
    struct arg_struct* args = (struct arg_struct*)arg;
    double* _t0 = args->arg1;
    double* _cur_t = args->arg2;
    queue* _recent_contacts_queue = args->arg3;
    queue* _close_contacts_queue = args->arg4;

    double last_checked_timestamp = -1;

    while (1) {
        usleep(500000);

        if (*_cur_t > END_TIME) break;

        /* -------------------------------------------------------------------------- */
        /*                                pop contacts                                */
        /* -------------------------------------------------------------------------- */

        if (!_close_contacts_queue->empty) {

            if (*_cur_t - _close_contacts_queue->buf[_close_contacts_queue->head]->timestamp > CLOSE_DEL_TIME) {

                queueDel(_close_contacts_queue);
            }
        }

        /* -------------------------------------------------------------------------- */
        /*                                add contacts                                */
        /* -------------------------------------------------------------------------- */

        if (!_recent_contacts_queue->empty) {

            int _last_added_index = _recent_contacts_queue->lastAddIndex;
            contact _last_added_cont;
            _last_added_cont.macaddress = _recent_contacts_queue->buf[_last_added_index]->macaddress;
            _last_added_cont.timestamp = _recent_contacts_queue->buf[_last_added_index]->timestamp;

            // if a new recent contact has been added, find the starting index for close contacts search 
            if (_last_added_cont.timestamp != last_checked_timestamp) {

                int _start_index = _recent_contacts_queue->head;

                for (int i = _last_added_index; ; i--) {

                    if (i < 0) {
                        i = _recent_contacts_queue->bufSize - 1;
                    }

                    if (_last_added_cont.timestamp - _recent_contacts_queue->buf[i]->timestamp > MAX_CLOSE_CONTACT_TIME) {
                        break;
                    }
                    else {
                        _start_index = i;
                    }

                    if (i == _recent_contacts_queue->head) {
                        break;
                    }
                }

                // check close contacts of last added contact
                for (int i = _start_index; ; i++) {

                    if (i == _recent_contacts_queue->bufSize) {
                        i = 0;
                    }

                    if (i == _last_added_index) {
                        break;
                    }

                    if (_last_added_cont.timestamp - _recent_contacts_queue->buf[i]->timestamp < MIN_CLOSE_CONTACT_TIME) {
                        break;
                    }

                    if (_recent_contacts_queue->buf[i]->macaddress == _last_added_cont.macaddress) {
                        // add close contact
                        contact* _close_contact = (contact*)malloc(sizeof(contact));
                        _close_contact->macaddress = _last_added_cont.macaddress;
                        _close_contact->timestamp = _last_added_cont.timestamp;
                        queueAdd(_close_contacts_queue, _close_contact);
                    }
                }

                last_checked_timestamp = _last_added_cont.timestamp;
            }
        }
    }

    return (NULL);
}