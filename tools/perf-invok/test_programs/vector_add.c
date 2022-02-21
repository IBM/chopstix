#include <stdlib.h>

void add(unsigned int N, int *a, int *b, int *c) {
    for (unsigned int i = 0; i < N; i++)
       c[i] = a[i] + b[i];
}

int main() {
    const unsigned int max_n = 100000000;
    int *a = malloc(sizeof(int) * max_n);
    int *b = malloc(sizeof(int) * max_n);
    int *c = malloc(sizeof(int) * max_n);

    for (unsigned int i = 0; i < 10; i++)
        add(i % 2 ? max_n : max_n / 100, a, b, c);

    return 0;
}
