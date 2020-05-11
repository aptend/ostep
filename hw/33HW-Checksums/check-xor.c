#include <stdio.h>
#include <unistd.h>

unsigned char check_xor(int fd) {
    char buf[4096];
    unsigned char ans = 0;
    int n = 0, i = 0;
    while ((n = read(0, &buf, 4096)) > 0)
    {
        for (i = 0; i < n; i++)
        {
            ans = ans ^ buf[i];
        }
    }
    return ans;
}

int main()
{
    printf("xor: %u\n", check_xor(0));
}
