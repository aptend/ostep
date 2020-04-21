#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int child = fork();
    if (0 == child)
    {
        close(STDOUT_FILENO);
        int n = write(STDOUT_FILENO, "12345\n", 6);
        fprintf(stderr, "[child] write %d bytes to stdout\n", n);
    }
    wait(NULL);
}
