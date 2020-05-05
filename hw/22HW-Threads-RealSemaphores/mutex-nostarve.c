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


typedef struct __ns_mutex_t {
    sem_t mutex;
    sem_t check_turn;
    int token;
    int turn;
} ns_mutex_t;

void ns_mutex_init(ns_mutex_t *m) {
    Sem_init(&m->mutex, 0, 1);
    Sem_init(&m->check_turn, 0, 0);
    m->token = 0;
    m->turn = 0;
}

void ns_mutex_acquire(ns_mutex_t *m) {
    Sem_wait(&m->mutex);
    int local_token = m->token;
    printf("get token %d\n", local_token);
    m->token++;
    while (local_token != m->turn) {
        printf("%d:%d\n not my turn, sleep", local_token, m->turn);
        Sem_wait(&m->check_turn);
        if (local_token != m->turn)
            Sem_post(&m->check_turn);
    }
    printf("execute at turn %d\n", m->turn);
    Sem_post(&m->mutex);
}



void ns_mutex_release(ns_mutex_t *m) {
    Sem_wait(&m->mutex);
    printf("finish at turn %d\n", m->turn);
    m->turn++;
    Sem_post(&m->check_turn);
    Sem_post(&m->mutex);
}

ns_mutex_t m;

void *worker(void *arg) {
    int thread_id = *(int *)arg;
    printf("thread %d ", thread_id);
    ns_mutex_acquire(&m);
    sleep(0.2);
    ns_mutex_release(&m);
    return NULL;
}



int main(int argc, char *argv[]) {
    printf("parent: begin\n");
    ns_mutex_init(&m);
    int N = 10;
    int i;
    pthread_t threads[N];
    for (i = 0; i < N; i++){
        int *arg = Malloc(sizeof(int));
        *arg = i;
        Pthread_create(&threads[i], NULL, worker, arg);
    }
    for (i = 0; i < N; i++)
        Pthread_join(threads[i], NULL);
    printf("parent: end\n");
    return 0;
}

