#include <stdlib.h>

int main()
{
    int *arr = calloc(5, sizeof(int));
    free(arr+2);
}
