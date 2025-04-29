#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdatomic.h>
#include "queue.h"

#define LOOP 150 // Number of times the producers will loop
#define ANG 10   // Size of the array to hold the angles
#define PRODSIZE 100
#define CONSIZE 100

// Structure for timing queue processing
typedef struct {
    struct timeval buf[LOOP * PRODSIZE];
    long head;
    pthread_mutex_t lock;
} timeQueue;

void pushTime(timeQueue *tq);
double calcTime(timeQueue start, timeQueue end);
void *consumer(void *args);
void *producer(void *args);
void *computeSine(void *arg);

timeQueue start;
timeQueue end;
atomic_int stop_flag = 0;

int main(int argc, char *argv[]) {

    // read command line arguments
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <number of producers> <number of consumers>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    int PRODSIZE = atoi(argv[1]);
    int CONSIZE = atoi(argv[2]);
    if (PRODSIZE <= 0 || CONSIZE <= 0) {
        fprintf(stderr, "Error: Number of producers and consumers must be positive integers.\n");
        exit(EXIT_FAILURE);
    }

    if (PRODSIZE > 1000 || CONSIZE > 1000) {
        fprintf(stderr, "Error: Number of producers and consumers must be less than or equal to 100.\n");
        exit(EXIT_FAILURE);
    }



    queue *fifo = queueInit();
    if (fifo == NULL) {
        fprintf(stderr, "Error: Queue initialization failed.\n");
        exit(EXIT_FAILURE);
    }

    pthread_t con[CONSIZE];
    pthread_t prod[PRODSIZE];

    // Create producer threads
    for (int i = 0; i < PRODSIZE; i++) {
        pthread_create(&prod[i], NULL, producer, fifo);
    }

    // Create consumer threads
    for (int i = 0; i < CONSIZE; i++) {
        pthread_create(&con[i], NULL, consumer, fifo);
    }

    // Wait for all producers to finish
    for (int i = 0; i < PRODSIZE; i++) {
        pthread_join(prod[i], NULL);
    }

    // Signal consumers to stop
    atomic_store(&stop_flag, 1);

    pthread_mutex_lock(fifo->mut);
    pthread_cond_broadcast(fifo->notEmpty);  // Wake up consumers
    pthread_mutex_unlock(fifo->mut);

    // Wait for all consumers to finish
    for (int i = 0; i < CONSIZE; i++) {
        pthread_join(con[i], NULL);
    }

    // calc and print average wait time in queue
    double avgWaitTime = calcTime(start, end) / (PRODSIZE * LOOP);
    printf("Average wait time: %f milliseconds\n", avgWaitTime);

    // Clean up
    pthread_mutex_destroy(&start.lock);
    pthread_mutex_destroy(&end.lock);
    queueDelete(fifo);

    return 0;
}


void *consumer(void *q) {
    queue *fifo = (queue *)q;

    while (1) {
        workFunction *d = (workFunction *)malloc(sizeof(workFunction));
        if (d == NULL) {
            perror("Error: Memory allocation failed for workFunction.");
            exit(EXIT_FAILURE);
        }

        pthread_mutex_lock(fifo->mut);
        while (fifo->empty) {
            if (atomic_load(&stop_flag)) {  // Stop condition
                pthread_mutex_unlock(fifo->mut);
                free(d);
                return NULL;
            }
            printf("Consumer: Queue is empty, waiting...\n");
            pthread_cond_wait(fifo->notEmpty, fifo->mut);
        }

        queueDel(fifo, d);
        pushTime(&end);
        // printf("Consumer: Consumed an item.\n");

        pthread_mutex_unlock(fifo->mut);
        pthread_cond_signal(fifo->notFull);

        // Process work function
        d->work(d->arg);
        free(d->arg);
        free(d);
    }
    return NULL;
}

void *producer(void *q) {
    queue *fifo = (queue *)q;

    for (int i = 0; i < LOOP; i++) {
        workFunction *wf = (workFunction *)malloc(sizeof(workFunction));
        if (wf == NULL) {
            perror("Error: Memory allocation failed for workFunction.");
            exit(EXIT_FAILURE);
        }

        double *angle = (double *)malloc(ANG * sizeof(double));
        if (angle == NULL) {
            perror("Error: Memory allocation failed for angle array.");
            exit(EXIT_FAILURE);
        }

        for (int i = 0; i < ANG; i++) {
            angle[i] = i * 10 + 35;
        }

        wf->work = computeSine;
        wf->arg = (void *)angle;

        pthread_mutex_lock(fifo->mut);
        while (fifo->full) {
            printf("Producer: Queue is full, waiting...\n");
            pthread_cond_wait(fifo->notFull, fifo->mut);
        }

        queueAdd(fifo, wf);
        pushTime(&start);
        // printf("Producer: Produced an item.\n");

        pthread_mutex_unlock(fifo->mut);
        pthread_cond_signal(fifo->notEmpty);
    }
    return NULL;
}

void *computeSine(void *arg) {
    double *angle = (double *)arg;
    for (int i = 0; i < ANG; i++) {
        double res = sin(angle[i] * M_PI / 180.0);
        (void)res; // Prevent unused variable warning
    }
    return NULL;
}

void pushTime(timeQueue *tq) {
    struct timeval tv;
    gettimeofday(&tv, NULL);

    pthread_mutex_lock(&tq->lock);
    tq->buf[tq->head % (LOOP * PRODSIZE)] = tv;
    tq->head++;
    pthread_mutex_unlock(&tq->lock);
}

double calcTime(timeQueue start, timeQueue end) {
    int secs = 0;
    float usecs = 0;
    for (int i = 0; i < LOOP; i++) {
        secs += end.buf[i].tv_sec - start.buf[i].tv_sec;
        usecs += end.buf[i].tv_usec - start.buf[i].tv_usec;
        if (usecs > 1e6) {
            secs++;
            usecs -= 1e6;
        }
    }
    return secs * 1e3 + usecs / 1e3;
    // return usecs;
}
