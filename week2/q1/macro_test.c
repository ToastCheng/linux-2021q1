#include <stdio.h>

#define max_safe(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define max(a,b) \
    ((a) > (b)) ? (a) : (b)

int main() {
    int a = 1, b = 2;
    int c = 1, d = 2;
    printf("safe: %d\n", max_safe(a++, b++));
    printf("not safe: %d\n", max(c++, d++));
    printf("a: %d, b: %d, c: %d, d: %d\n", a, b, c, d);
    return 0;
}