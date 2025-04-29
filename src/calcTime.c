#include "calcTime.h"

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
