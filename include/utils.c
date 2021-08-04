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
{
    FILE* fptr;
    int size;

    fptr = fopen("close_contacts.bin", "rb");
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

void read_bin(const char* filename)
{
    int size;
    FILE* _fptr;

    size = bin_file_size(filename) / sizeof(contact);
    _fptr = fopen(filename, "rb");
    if (_fptr == NULL) {
        printf("Error!");
        exit(1);
    }

    contact* uploaded_contacts = (contact*)malloc(size * sizeof(contact));

    size_t elements_read = fread(uploaded_contacts, sizeof(contact), size, _fptr);
    if (elements_read != size) {
        printf("Error!");
        exit(1);
    }

    fclose(_fptr);

    for (int i = 0; i < size; i++) {
        printf("MAC Address: %lu\tTimestamp: %lf\n", (unsigned long)uploaded_contacts[i].macaddress, uploaded_contacts[i].timestamp);
    }

    free(uploaded_contacts);

    return;
}