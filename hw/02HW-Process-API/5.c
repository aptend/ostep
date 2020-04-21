#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int child = fork();
    if (0 == child)
    {
        printf("child wait return value: %d\n", wait(NULL));
    }
    else
    {
        printf("spawn a child process %d\n", child);
        int child_done;
        if ((child_done = wait(NULL)) > 0)
        {
            printf("child %d done\n", child_done);
        }
    }
}
