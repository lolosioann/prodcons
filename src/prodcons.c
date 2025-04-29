#include "prodcons.h"

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