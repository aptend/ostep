#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (0 == fork())
    {
        execl("/bin/ls", "ls", (char *)NULL);
    }

    if (0 == fork())
    {
        execlp("ls", "ls", (char *)NULL);
    }

    if (0 == fork())
    {
        char *env[1] = {NULL};
        execle("/bin/ls", "ls", (char *)NULL, env);
    }

    if (0 == fork())
    {
        char *args[2] = {"ls", NULL};
        execv("/bin/ls", args);
    }

    if (0 == fork())
    {
        char *args[2] = {"ls", NULL};
        execvp("ls", args);
    }

    if (0 == fork())
    {
        char *env[1] = {NULL};
        char *args[2] = {"ls", NULL};
        execve("/bin/ls", args, env);
    }

    while (wait(NULL) > 0)
        ;
}
