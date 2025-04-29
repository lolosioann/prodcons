#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "prodcons.h"
#include "calcTime.h"
#include "globals.h"

int main() {
    // Initialize the start and end time queues
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
