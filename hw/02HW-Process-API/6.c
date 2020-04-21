#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int child = fork();
    if (child > 0)
    {
        printf("spawn a child process %d\n", child);
        int child_done;
        while ((child_done = waitpid(-1, NULL, WNOHANG)) <= 0)
        {
            printf("wait child  ");
        }
        printf("\nchild done %d\n", child_done);

    }
}
