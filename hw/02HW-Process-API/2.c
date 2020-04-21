#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    int fd = open("./3.output", O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
    int LEN = 5200; // greater than 2
    int child = fork();
    if (0 == child)
    {
        char buf[LEN];
        for (int i=0;i<LEN;i++) {
            buf[i] = 'c';
        }
        buf[LEN-2] = '\n';
        buf[LEN-1] = '\0';
        write(fd, buf, strlen(buf));
        write(fd, buf, strlen(buf));
    }
    else
    {
        char buf[LEN];
        for (int i = 0; i < LEN; i++)
        {
            buf[i] = 'p';
        }
        buf[LEN - 2] = '\n';
        buf[LEN - 1] = '\0';
        write(fd, buf, strlen(buf));
        write(fd, buf, strlen(buf));
    }
    wait(NULL);
}
