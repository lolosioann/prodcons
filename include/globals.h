#ifndef GLOBALS_H
#define GLOBALS_H

#include <sys/time.h>
#include <stdatomic.h>
#include <pthread.h>

#define LOOP 150 // Number of times the producers will loop
#define ANG 10   // Size of the array to hold the angles
#define PRODSIZE 100
#define CONSIZE 10

// Structure for timing queue processing
typedef struct {
    struct timeval buf[LOOP * PRODSIZE];
    long head;
    pthread_mutex_t lock;
} timeQueue;

extern timeQueue start;
extern timeQueue end;
extern atomic_int stop_flag;


#endif /* GLOBALS_H */