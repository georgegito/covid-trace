/* -------------------------------------------------------------------------- */
/*                                   queue.c                                  */
/* -------------------------------------------------------------------------- */

queue* queueInit(int bufSize)
{
  queue* q;

  q = (queue*)malloc(sizeof(queue));
  if (q == NULL) return (NULL);

  q->buf = (contact**)malloc(bufSize * sizeof(contact*));
  q->empty = 1;
  q->full = 0;
  q->head = 0;
  q->tail = 0;
  q->lastAddIndex = -1;
  q->bufSize = bufSize;
  q->mut = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(q->mut, NULL);
  q->notFull = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
  pthread_cond_init(q->notFull, NULL);
  q->notEmpty = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
  pthread_cond_init(q->notEmpty, NULL);

  return (q);
}

void queueDelete(queue* q)
{
  free(q->buf);
  pthread_mutex_destroy(q->mut);
  free(q->mut);
  pthread_cond_destroy(q->notFull);
  free(q->notFull);
  pthread_cond_destroy(q->notEmpty);
  free(q->notEmpty);
  free(q);
}

void queueAdd(queue* q, contact* in)
{
  if (q->full)
    printf("Warning! Queue is full! \n");

  q->buf[q->tail] = in;
  q->lastAddIndex = q->tail;
  q->tail++;

  if (q->tail == q->bufSize)
    q->tail = 0;
  if (q->tail == q->head)
    q->full = 1;
  q->empty = 0;

  return;
}

void queueDel(queue* q)
{
  q->head++;
  if (q->head == q->bufSize)
    q->head = 0;
  if (q->head == q->tail)
    q->empty = 1;
  q->full = 0;

  return;
}