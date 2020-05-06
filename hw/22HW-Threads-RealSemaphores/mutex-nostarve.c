#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "common_threads.h"

//
// Here, you have to write (almost) ALL the code. Oh no!
// How can you show that a thread does not starve
// when attempting to acquire this mutex you build?
//

typedef struct __ns_mutex_t
{
    sem_t mutex;
    int room1;
    int room2;
    sem_t t1;
    sem_t t2;
} ns_mutex_t;

void ns_mutex_init(ns_mutex_t *m)
{
    Sem_init(&m->mutex, 0, 1);
    Sem_init(&m->t1, 0, 1);
    Sem_init(&m->t2, 0, 0);
    m->room1 = 0;
    m->room2 = 0;
}

void ns_mutex_acquire(ns_mutex_t *m)
{
    Sem_wait(&m->mutex);
    m->room1++;
    Sem_post(&m->mutex);

    Sem_wait(&m->t1);
    m->room2++;

    Sem_wait(&m->mutex);
    m->room1--;
    if (m->room1 == 0)
    {
        Sem_post(&m->t2);
    }
    else
    {
        Sem_post(&m->t1);
    }
    Sem_post(&m->mutex);

    Sem_wait(&m->t2);
    m->room2--;
}

void ns_mutex_release(ns_mutex_t *m)
{
    if (m->room2 == 0)
    {
        Sem_post(&m->t1);
    }
    else
    {
        Sem_post(&m->t2);
    }
}

ns_mutex_t m;
int counter = 0;

void *worker(void *arg)
{
    int thread_id = *(int *)arg;
    printf("thread%d is working\n", thread_id);
    ns_mutex_acquire(&m);
    counter++;
    ns_mutex_release(&m);
    return NULL;
}

int main(int argc, char *argv[])
{
    assert(argc == 2);
    int N = atoi(argv[1]);
    printf("parent: begin\n");
    ns_mutex_init(&m);
    int args[N];
    int i;
    pthread_t threads[N];
    for (i = 0; i < N; i++)
    {
        args[i] = i;
        Pthread_create(&threads[i], NULL, worker, &args[i]);
    }
    for (i = 0; i < N; i++)
        Pthread_join(threads[i], NULL);
    printf("counter %d\n", counter);
    printf("parent: end\n");
    return 0;
}
