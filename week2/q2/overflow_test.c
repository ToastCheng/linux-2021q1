#include <stdio.h>
#include <stdint.h>

uint16_t func(uint16_t N) {
    /* change all right side bits to 1 */
    N |= N >> 1;
    N |= N >> /* X */ 2;
    N |= N >> /* Y */ 4;
    N |= N >> /* Z */ 8;

    return (N + 1) >> 1;
}

uint16_t func2(uint16_t N) {
    return 1 << (31 - __builtin_clz(N));
}

int main() {
    for (int _i = 0; _i <= 65535; _i++) {
        uint16_t i = _i;
        uint16_t a = func(i);
        uint16_t b = func2(i);
        if (a != b) {
            printf("Assertion failed at %u: %u != %u\n", i, a, b);
        }
    }
}