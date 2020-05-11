#include <stdio.h>
#include <unistd.h>


unsigned short check_fletcher(int fd) {
    char buf[4096];
    unsigned char a = 0, b = 0;
    int n = 0, i = 0;
    while ((n = read(fd, &buf, 4096)) > 0)
    {
        for (i = 0; i < n; i++)
        {
            a = (a + buf[i]) % 255;
            b = (b + a) % 255;
        }
    }
    unsigned short ans = 0;
    ans |= a;
    ans = (ans << 8) | b;
    return ans;
}

int main()
{
    unsigned short ans = check_fletcher(0);
    unsigned b = ans & 0xFF;
    unsigned a = (ans >> 8) & 0xFF;
    printf("fletcher: %u, %u\n", a, b);
}
