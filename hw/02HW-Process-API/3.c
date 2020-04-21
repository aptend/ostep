#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

// wait(NULL) version
// int main(int argc, char *argv[])
// {
//     int child = fork();
//     if (0 == child)
//     {
//         printf("hello\n");
//     }
//     else
//     {
//         wait(NULL);
//         printf("world\n");
//     }
// }

int main(int argc, char *argv[])
{
    int pipefd[2];
    char buf;

    pipe(pipefd);

    int child = fork();
    if (0 == child)
    {
        close(pipefd[0]); // close read end
        printf("hello\n");
        printf(" [child] sleep 2 seconds to pretend working hard\n");
        sleep(2);
        close(pipefd[1]); // drop the pipe, read end will return -1
    } else {
        close(pipefd[1]); // close write end
        while (read(pipefd[0], &buf, 1) > 0) // receive -1
            ;
        printf("world\n");
        close(pipefd[0]);
    }
    while (wait(NULL) > 0)
      ;
}
