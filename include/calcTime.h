#ifndef CALCTIME_H
#define CALCTIME_H

#include "globals.h"
#include <pthread.h>

void pushTime(timeQueue *tq);
double calcTime(timeQueue start, timeQueue end);

#endif /* CALCTIME_H */