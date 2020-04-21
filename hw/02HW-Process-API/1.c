#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    int wait_child = 0;
    if (argc >= 2)
    {
        wait_child = 1;
    }
    int x = 41;
    printf("original x: %d\n", x);
    int child = fork();
    if (0 == child)
    {
        printf("child read x: %d\n", x);
        x = 42;
        printf("child x modified: %d\n", x);
    }
    else
    {
        if (wait_child)
        {
            int child_done;
            if ((child_done = wait(NULL)) > 0)
            {
                printf("child %d done\n", child_done);
            }
        }
        printf("parent read x: %d\n", x);
        x = 47;
        printf("parent x modified: %d\n", x);
    }
}
