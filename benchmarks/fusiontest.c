#include <stdio.h>

__attribute__((noinline))
void run()
{
    int a[1000], b[1000];
    for (int i = 0; i < 1000; ++i)
    {
        a[i] = i * i;
    }
    for (int i = 0; i < 1000; ++i)
    {
        printf("%d", a[i] / 2);
        b[i] = a[i] / 2;
    }
    int i = 1;
    if (i)
    {
        puts("Hi");
    }
    for (int i = 0; i < 1000; ++i)
    {
        printf("%d %d\n", a[i], b[i]);
    }
}
int main() {
    run();
}