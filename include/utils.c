/* -------------------------------------------------------------------------- */
/*                                   utils.c                                  */
/* -------------------------------------------------------------------------- */

void cont_prt(queue* q)
{
    if (q->empty == 1) {

        printf("No contacts\n");
        return;
    }

    int i = q->head;
    do {
        if (i == q->bufSize) {
            i = 0;
            if (i == q->tail)
                break;
        }

        printf("MAC Address: %lu\tTimestamp: %lf\n", (unsigned long)q->buf[i]->macaddress, q->buf[i]->timestamp);
        i++;
    } while (i != q->tail);
}

int bin_file_size(const char* filename)
// returns the size of a binary file in bytes
{
    FILE* fptr;
    int size;

    fptr = fopen(filename, "rb");
    if (fptr == NULL) {
        printf("Error!");
        exit(1);
    }

    fseek(fptr, 0, SEEK_END);
    size = ftell(fptr);
    fseek(fptr, 0, SEEK_SET);

    fclose(fptr);

    return size;
}

void read_bin()
{
    int num_of_uploads;
    int num_of_up_conts;
    FILE* _fptr1;
    FILE* _fptr2;
    FILE* _fptr3;
    size_t elements_read;

    num_of_uploads = bin_file_size("upload_times.bin") / sizeof(double);
    num_of_up_conts = bin_file_size("close_contacts.bin") / sizeof(contact);

    contact* uploaded_contacts = (contact*)malloc(num_of_up_conts * sizeof(contact));
    double* upload_times = (double*)malloc(num_of_uploads * sizeof(double));
    int* contacts_nums = (int*)malloc(num_of_uploads * sizeof(int));

    _fptr1 = fopen("close_contacts.bin", "rb");
    _fptr2 = fopen("upload_times.bin", "rb");
    _fptr3 = fopen("contacts_nums.bin", "rb");

    if (_fptr1 == NULL || _fptr2 == NULL || _fptr3 == NULL) {
        printf("Error!");
        exit(1);
    }

    elements_read = fread(uploaded_contacts, sizeof(contact), num_of_up_conts, _fptr1);
    if (elements_read != num_of_up_conts) {
        printf("Error!");
        exit(1);
    }

    elements_read = fread(upload_times, sizeof(double), num_of_uploads, _fptr2);
    if (elements_read != num_of_uploads) {
        printf("Error!");
        exit(1);
    }

    elements_read = fread(contacts_nums, sizeof(int), num_of_uploads, _fptr3);
    if (elements_read != num_of_uploads) {
        printf("Error!");
        exit(1);
    }

    fclose(_fptr1);
    fclose(_fptr2);
    fclose(_fptr3);

    int _index = 0;
    for (int i = 0; i < num_of_uploads; i++) {
        printf("- Upload time: %lf\t Contacts uploaded: %d\n", upload_times[i], contacts_nums[i]);
        for (int j = 0; j < contacts_nums[i]; j++) {
            printf("\tMAC Address: %lu\tTimestamp: %lf\n", (unsigned long)uploaded_contacts[_index].macaddress, uploaded_contacts[_index].timestamp);
            _index++;
        }
    }

    if (_index != num_of_up_conts) {
        printf("Error!");
        exit(2);
    }

    free(uploaded_contacts);
    free(upload_times);
    free(contacts_nums);

    return;
}