#include <stdio.h>

typedef struct {
    int a;
    double b;
    int c;
} s_t;

typedef struct {
    int a;
    int b;
    double c;
} s_t2;

int main() {
    s_t s1 = {0, 0, 0};
    printf("s_t:\n%p\n%p\n%p\n", &s1.a, &s1.b, &s1.c);
    printf("size: %lu\n", sizeof(s1));
    s_t2 s2 = {0, 0, 0};
    printf("s_t2:\n%p\n%p\n%p\n", &s2.a, &s2.b, &s2.c);
    printf("size: %lu\n", sizeof(s2));
}