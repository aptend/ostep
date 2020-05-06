#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "common_threads.h"

//
// Your code goes in the structure and functions below
//

typedef struct __rwlock_t
{
    sem_t mutex;
    sem_t writer_lock;
    sem_t reader_lock;
    int reader;
} rwlock_t;

void rwlock_init(rwlock_t *rw)
{
    Sem_init(&rw->mutex, 0, 1);
    Sem_init(&rw->writer_lock, 0, 1);
    Sem_init(&rw->reader_lock, 0, 1);
    rw->reader = 0;
}

void rwlock_acquire_readlock(rwlock_t *rw)
{
    Sem_wait(&rw->reader_lock);
    Sem_post(&rw->reader_lock);

    Sem_wait(&rw->mutex);
    rw->reader++;
    if (rw->reader == 1) {
        Sem_wait(&rw->writer_lock);
    }
    Sem_post(&rw->mutex);
}

void rwlock_release_readlock(rwlock_t *rw)
{
    Sem_wait(&rw->mutex);
    rw->reader--;
    if (rw->reader == 0)
    {
        Sem_post(&rw->writer_lock);
    }
    Sem_post(&rw->mutex);
}

void rwlock_acquire_writelock(rwlock_t *rw)
{
    Sem_wait(&rw->reader_lock);
    Sem_wait(&rw->writer_lock);
}

void rwlock_release_writelock(rwlock_t *rw)
{
    Sem_post(&rw->reader_lock);
    Sem_post(&rw->writer_lock);
}

//
// Don't change the code below (just use it!)
//

// int loops;
// int value = 0;

// rwlock_t lock;

// void *reader(void *arg)
// {
//     int i;
//     for (i = 0; i < loops; i++)
//     {
//         rwlock_acquire_readlock(&lock);
//         printf("read %d\n", value);
//         rwlock_release_readlock(&lock);
//     }
//     return NULL;
// }

// void *writer(void *arg)
// {
//     int i;
//     for (i = 0; i < loops; i++)
//     {
//         rwlock_acquire_writelock(&lock);
//         value++;
//         printf("write %d\n", value);
//         rwlock_release_writelock(&lock);
//     }
//     return NULL;
// }

// int main(int argc, char *argv[])
// {
//     assert(argc == 4);
//     int num_readers = atoi(argv[1]);
//     int num_writers = atoi(argv[2]);
//     loops = atoi(argv[3]);

//     pthread_t pr[num_readers], pw[num_writers];

//     rwlock_init(&lock);

//     printf("begin\n");

//     int i;
//     for (i = 0; i < num_readers; i++)
//         Pthread_create(&pr[i], NULL, reader, NULL);
//     for (i = 0; i < num_writers; i++)
//         Pthread_create(&pw[i], NULL, writer, NULL);

//     for (i = 0; i < num_readers; i++)
//         Pthread_join(pr[i], NULL);
//     for (i = 0; i < num_writers; i++)
//         Pthread_join(pw[i], NULL);

//     printf("end: value %d\n", value);

//     return 0;
// }
int loops;
int value = 0;

rwlock_t lock;

void *reader(void *arg)
{
    int i;
    usleep(*(int *)arg * 100 * 1000);
    for (i = 0; i < loops; i++)
    {
        rwlock_acquire_readlock(&lock);
        printf("read %d\n", value);
        usleep(300 * 1000);
        rwlock_release_readlock(&lock);
    }
    return NULL;
}

void *writer(void *arg)
{
    int i;
    usleep(200 * 1000);
    for (i = 0; i < loops; i++)
    {
        rwlock_acquire_writelock(&lock);
        value++;
        printf("write %d\n", value);
        rwlock_release_writelock(&lock);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    assert(argc == 4);
    int num_readers = atoi(argv[1]);
    int num_writers = atoi(argv[2]);
    loops = atoi(argv[3]);

    pthread_t pr[num_readers], pw[num_writers];
    int rargs[num_readers], wargs[num_writers];
    rwlock_init(&lock);

    printf("begin\n");

    int i;
    for (i = 0; i < num_readers; i++)
    {
        rargs[i] = i;
        Pthread_create(&pr[i], NULL, reader, &rargs[i]);
    }
    for (i = 0; i < num_writers; i++)
    {
        wargs[i] = i;
        Pthread_create(&pw[i], NULL, writer, &wargs[i]);
    }

    for (i = 0; i < num_readers; i++)
        Pthread_join(pr[i], NULL);
    for (i = 0; i < num_writers; i++)
        Pthread_join(pw[i], NULL);

    printf("end: value %d\n", value);

    return 0;
}
