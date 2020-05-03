#include "mythreads.h"
#include <pthread.h>

#define NUMCORES 4


typedef struct counter_t {
    int global;
    pthread_mutex_t global_lock;
    int locals[NUMCORES];
    pthread_mutex_t local_locals[NUMCORES];
} counter_t
