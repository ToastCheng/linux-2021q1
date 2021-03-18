#include <stdio.h>
#include <stdint.h>
#include <limits.h>

uint32_t func(uint32_t N) {
    /* change all right side bits to 1 */
    N |= N >> 1;
    N |= N >> /* X */ 2;
    N |= N >> /* Y */ 4;
    N |= N >> /* Z */ 8;
    N |= N >> /* Z */ 16;

    return (N + 1) >> 1;
}

uint32_t func2(uint32_t N) {
    return 1 << (31 - __builtin_clz(N));
}

int main() {
    uint64_t max = (uint64_t) INT_MAX + 10;
    for (uint64_t _i = 1; _i <= max; _i++) {
        uint32_t i = _i;
        uint32_t a = func(i);
        uint32_t b = func2(i);
        if (a != b) {
            printf("Assertion failed at %u: %u != %u\n", i, a, b);
        }
    }
}