#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdatomic.h>
#include "queue.h"

#define LOOP 10
#define ANG 10 // the size of the array to hold the angles
#define PRODSIZE 1
#define CONSIZE 1

typedef struct {
    struct timeval buf[LOOP * PRODSIZE];
    long head;
    pthread_mutex_t lock;
} timeQueue; // to time the waiting time in queue

void pushTime(timeQueue *tq);
double calcTime(timeQueue start, timeQueue end);

void *consumer (void *args);
void *producer (void *args);
void *computeSine(void *arg);
timeQueue start;
timeQueue end;
atomic_int stop_flag = 0;

int main() {
    // initialize
    queue *fifo;
    pthread_t con[CONSIZE];
    pthread_t prod[PRODSIZE];
    // double angle = 45;

    // initialize queue
    fifo = queueInit ();
    if (fifo ==  NULL) {
        fprintf (stderr, "main: Queue Init failed.\n");
        exit (1);
    }
    // create producer threads
    for (int i = 0; i < PRODSIZE; i++) {
        pthread_create(&prod[i], NULL, producer, fifo);
    }
    // create consumer threads
    for (int i = 0; i < CONSIZE; i++) {
        pthread_create(&con[i], NULL, consumer, fifo);
    }
    // pthread_create(&con, NULL, producer, fifo);
    // create consumer threads
    // pthread_create(&prod, NULL, consumer, fifo);
    // join threads
    for (int i = 0; i < PRODSIZE; i++) {
        pthread_join(prod[i], NULL);
    }
    atomic_store(&stop_flag, 1);
    for (int i = 0; i < CONSIZE; i++) {
        pthread_join(con[i], NULL);
    }
    // pthread_join(prod, NULL);
    // pthread_join(con, NULL);
    pthread_mutex_destroy(&start.lock);
    pthread_mutex_destroy(&end.lock);
    double avgWaitTime = calcTime(start, end) / (PRODSIZE * LOOP);
    printf("Average wait time: %f\n", avgWaitTime);
    queueDelete(fifo);
    return 0;
}

void *consumer (void *q){
    queue *fifo;
    
    fifo = (queue *)q;
    // for (int i = 0; i < LOOP; i++) {
    while (1) {
        workFunction *d = (workFunction *)malloc(sizeof(workFunction));
        pthread_mutex_lock (fifo->mut);
        while (fifo->empty) {
            if (atomic_load(&stop_flag)) {
                pthread_mutex_unlock (fifo->mut);
                return NULL;
            }
            printf ("consumer: queue EMPTY.\n");
            pthread_cond_wait (fifo->notEmpty, fifo->mut);
        }
        queueDel(fifo, d);
        // pushTime(&end);
        pthread_mutex_unlock (fifo->mut);
        pthread_cond_signal (fifo->notFull);
        d->work(d->arg);
        free(d->arg);
        free(d);
    }
    return (NULL);
}

void *producer (void *q) {
    queue *fifo;
    fifo = (queue *)q;

    for (int i = 0; i < LOOP; i++) {
        // produce something ...
        workFunction *wf = (workFunction *)malloc(sizeof(workFunction));
        double *angle = (double *) malloc(ANG*sizeof(double));
        for (int i = 0; i < ANG; i++) {
            angle[i] = i * 10 + 35;
        }
        // *angle = 45;
        wf->work = computeSine;
        wf->arg = (void *) angle;

        // place in queue
        pthread_mutex_lock (fifo->mut);
        while (fifo->full) {
            printf ("producer: queue FULL.\n");
            pthread_cond_wait (fifo->notFull, fifo->mut);
        }
        queueAdd(fifo, wf);
        pushTime(&start);
        pthread_mutex_unlock (fifo->mut);
        pthread_cond_signal (fifo->notEmpty);
    }   
    return (NULL); 
}

 void *computeSine(void *arg) {
    double *angle = (double *)arg;
    double res;
    for (int i = 0; i < ANG; i++) {
        res = sin(angle[i] * 3.14159 / 180);
    }
    return NULL;
}

void pushTime(timeQueue *tq) {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    pthread_mutex_lock(&tq->lock);  // Ensure atomic update
    tq->buf[tq->head % (LOOP * PRODSIZE)] = tv;  // Prevent overflow
    tq->head++;
    pthread_mutex_unlock(&tq->lock);
}

double calcTime(timeQueue start, timeQueue end) {
    int secs = 0;
    int usecs = 0;
    for (int i = 0; i < LOOP; i++) {
        secs += end.buf[i].tv_sec - start.buf[i].tv_sec;
        usecs += end.buf[i].tv_usec - start.buf[i].tv_usec;
        if (usecs > 1e6) {
            secs++;
            usecs -= 1e6;
        }
    }
    return secs + usecs / 1000000.0;
}