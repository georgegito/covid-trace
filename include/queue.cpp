// void *producer(void *q)
// {
//   queue *fifo;
//   fifo = (queue *)q;

//   for (int i = 0; i < WORKS_PER_PRO; i++) {
//     struct workFunction * _func = (struct workFunction *)malloc(sizeof(struct workFunction));
//     _func->work = (void *)print;
//     _func->arg = malloc(sizeof(int));
//     pthread_mutex_lock(fifo->mut);
//     while (fifo->full) {
//       // printf ("Producer: queue FULL.\n");
//       pthread_cond_wait(fifo->notFull, fifo->mut);
//     }

//     *((int*)_func->arg) = ++workIndex;
//     queueAdd(fifo, _func);
    
//     pthread_mutex_unlock(fifo->mut);
//     pthread_cond_signal(fifo->notEmpty);
//   }

//   // mutex to avoid data race
//   pthread_mutex_lock(fifo->mut);
//   proFinished++;
//   pthread_mutex_unlock(fifo->mut);

//   // the last producer sends signals to unblock the consumers when they finish theirs works
//   if (proFinished == fifo->NUM_OF_PRO) {
//     // printf("I am the last producer.\n");
//     while (conFinished != fifo->NUM_OF_CON) {
//       // printf("The last producer sends signal to unblock the blocked consumers.\n");
//       pthread_cond_broadcast(fifo->notEmpty);
//     }
//   }

//   return (NULL);
// }

// void *consumer(void *q)
// {
//   queue *fifo;
//   fifo = (queue *)q;

//   while(1) {
//     struct workFunction * exec_func;
//     pthread_mutex_lock(fifo->mut);
//     while (fifo->empty) {
//       // printf ("Consumer: queue EMPTY.\n");
//       if (fifo->empty && proFinished == fifo->NUM_OF_PRO) {
//         conFinished++;
//         pthread_mutex_unlock (fifo->mut);
//         // printf("A consumer finished all of its works (%d consumers total).\n", conFinished);
//         return (NULL);
//       }
//       pthread_cond_wait(fifo->notEmpty, fifo->mut);
//     }

//     queueDel(fifo, &exec_func);
//     // printf("A consumer removed work %d from the queue.\n", *((int*)exec_func->arg));

// /* --------------------------- function execution --------------------------- */
//     ((void(*)())exec_func->work)(*(int *)exec_func->arg);
// /* -------------------------------------------------------------------------- */

//     pthread_mutex_unlock (fifo->mut);
//     pthread_cond_signal (fifo->notFull);
//   }
// }

queue *queueInit()
{
  queue *q;

  q = (queue *)malloc(sizeof (queue));
  if (q == NULL) return (NULL);

  q->empty = 1;
  q->full = 0;
  q->head = 0;
  q->tail = 0;
  q->mut = (pthread_mutex_t *)malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(q->mut, NULL);
  q->notFull = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
  pthread_cond_init(q->notFull, NULL);
  q->notEmpty = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
  pthread_cond_init(q->notEmpty, NULL);

  return (q);
}

void queueDelete(queue *q)
{
  pthread_mutex_destroy(q->mut);
  free(q->mut);	
  pthread_cond_destroy(q->notFull);
  free(q->notFull);
  pthread_cond_destroy(q->notEmpty);
  free(q->notEmpty);
  free(q);
}

void queueAdd(queue *q,  contact *in)
{
  if (q->full)
    printf("Warning! Queue is full! \n");

  q->buf[q->tail] = in;
  q->tail++;

  if (q->tail == QUEUESIZE)
    q->tail = 0;
  if (q->tail == q->head)
    q->full = 1;
  q->empty = 0;

  return;
}

void queueDel(queue *q)
{
  q->head++;
  if (q->head == QUEUESIZE)
    q->head = 0;
  if (q->head == q->tail)
    q->empty = 1;
  q->full = 0;

  return;
}