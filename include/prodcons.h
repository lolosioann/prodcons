#ifndef PRODCONS_H
#define PRODCONS_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
#include "globals.h"
#include "calcTime.h"

void *consumer(void *q);
void *producer(void *q);
void *computeSine(void *arg);

#endif /* PRODCONS_H */