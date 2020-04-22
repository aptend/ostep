#define _GNU_SOURCE
#include <sched.h>
#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define TIMES 1000000

long pref()
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    // return result in nano seconds.
    return spec.tv_sec * 1000000000 + spec.tv_nsec;
}

int main(int argc, char *argv[])
{
    int rfd[2];
    int lfd[2];

    pipe(rfd);
    pipe(lfd);

    if (0 == fork())
    {
        close(lfd[0]); // close read end
        close(rfd[1]); // close read end
        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(1, &mask);
        if (sched_setaffinity(0, sizeof(mask), &mask) < 0)
        {
            perror("sched_setaffinity failed");
        }
        printf("child current affinity: %08x\n", *(uint32_t *)&mask);
        char buf;
        int round;
        for (round=0; round < TIMES; round++) {
            write(lfd[1], "x", 1);
            read(rfd[0], &buf, 1);
        }
        exit(0);
    }

    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(1, &mask);

    if (sched_setaffinity(0, sizeof(mask), &mask) < 0)
    {
        perror("sched_setaffinity failed");
    }
    printf("parent current affinity: %08x\n", *(uint32_t *)&mask);
    close(lfd[1]);  // close read end
    close(rfd[0]); // close read end
    char buf;
    int round;
    long start = pref();
    for (round = 0; round < TIMES; round++)
    {
        write(rfd[1], "", 1);
        read(lfd[0], &buf, 1);
    }
    long delta = pref() - start;
    printf("%ldns\n", delta / TIMES);

    while (wait(NULL) > 0);
}
