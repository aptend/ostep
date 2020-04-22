
#include <linux/types.h>
#include <sys/time.h>
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
    clo_g
}

int main()
{
    int fd = open("sample", O_RDONLY);
    int i, round;
    read(fd, NULL, 0);
    for (round = 0; round < 10; round++)
    {
        __u64 start = rdtsc();
        for (i = 0; i < TIMES; i++)
        {
            read(fd, NULL, 0);
        }
        __u64 end = rdtsc();

        printf("%d times read: %llu\n", TIMES, end - start);
    }
}
