#include <stdio.h>

extern unsigned long long sandwich_return();

int main() {
    unsigned long long result = sandwich_return();
    printf("%016llx\n", result);
    return 0;
}
