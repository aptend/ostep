
#include <linux/types.h>
#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define TIMES 1000000

__u64 rdtsc()
{
    __u32 lo, hi;

    __asm__ __volatile__("rdtsc": "=a"(lo), "=d"(hi));
    return (__u64)hi << 32 | lo;
}

__u64 pref()
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    // return result in nano seconds.
    return spec.tv_sec * 1000000000 + spec.tv_nsec;
}

int main()
{
    int i, round;
    int fd = open("sample", O_RDONLY);
    __u64 total = 0;
    for (round = 0; round < 10; round++)
    {
        __u64 start = pref();
        for (i = 0; i < TIMES; i++)
        {
            read(fd, NULL, 0);
        }
        __u64 end = pref();

        __u64 delta = end - start;
        total += delta;
        printf("%d times read cost: %llu\n", TIMES, delta);
    }
    printf("average: %llu\n", total / 10);

}
