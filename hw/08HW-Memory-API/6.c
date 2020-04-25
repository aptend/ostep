#include <stdlib.h>
#include <stdio.h>
int main()
{
    int *arr = malloc(5*sizeof(int));
    free(arr);
    arr[2] = 42;
    printf("%d\n", arr[2]);
}
