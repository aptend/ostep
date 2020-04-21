#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int pipefd[2];

    pipe(pipefd);

    fprintf(stderr, "[e] create producer\n");
    if (0 == fork()) // child 1, producer
    {
        close(pipefd[0]);               // close read end
        dup2(pipefd[1], STDOUT_FILENO); // chars to stdout are redirected to pipefd[1]

        // call producer program;
        printf("[child 1] hello\n");
        printf("[child 1] sleep 2 seconds to pretend working hard\n");
        exit(0); // OS will close fd
    }

    fprintf(stderr, "[e] create consumer\n");
    if (0 == fork()) // child 2, consumer
    {
        close(pipefd[1]);              // close write end
        dup2(pipefd[0], STDIN_FILENO); // read request to stdin will be redirected to pipefd[0]

        // call consumer program
        char buf[1000];
        while (fgets(buf, 1000, stdin))
        {
            printf("[child2 received] %s", buf);
        }
        exit(0); // OS will close fd
    }

    close(pipefd[0]);
    close(pipefd[1]);
    fprintf(stderr, "[e] main thread waits\n");

    while (wait(NULL) > 0)
        ;
}
