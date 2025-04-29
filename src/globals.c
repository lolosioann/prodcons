// src/globals.c
#include "globals.h"

timeQueue start = { .head = 0, .lock = PTHREAD_MUTEX_INITIALIZER };
timeQueue end   = { .head = 0, .lock = PTHREAD_MUTEX_INITIALIZER };
atomic_int stop_flag = 0;

// timeQueue start;
// timeQueue end;
// atomic_int stop_flag = 0;