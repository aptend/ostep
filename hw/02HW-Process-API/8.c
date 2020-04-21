#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int pipefd[2];

    pipe(pipefd);

    if (0 == fork()) // child 1, producer
    {
        // printf("hello\n");
        close(pipefd[0]);               // close read end
        if (dup2(pipefd[1], STDOUT_FILENO) < 0) {
            printf("bad");
        } // chars to stdout are redirected to pipefd[1]

        // call producer program;
        printf("hello\n");
        printf("[child 1] sleep 2 seconds to pretend working hard\n");
    }

    if (0 == fork()) // child 1, producer
    {
        close(pipefd[1]);              // close write end
        dup2(pipefd[0], STDIN_FILENO); // read request to stdin will be redirected to pipefd[0]

        // call consumer program
        char buf[1000];
        while (fgets(buf, 1000, stdin))
        {
            printf("[child2 received] %s", buf);
        }
    }

    while (wait(NULL) > 0)
        ;
}
