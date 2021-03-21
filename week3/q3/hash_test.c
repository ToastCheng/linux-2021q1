#include <stdio.h>
#include <stdint.h>

static inline uint32_t hash_blob(const char *buffer, size_t len)
{
    const uint8_t *ptr = (const uint8_t *) buffer;
    size_t h = len;
    size_t step = (len >> 5) + 1;
    for (size_t i = len; i >= step; i -= step)
        h = h ^ ((h << 5) + (h >> 2) + ptr[i - 1]);
    return h == 0 ? 1 : h % 1024;
}

int main() {
    char *str[3] = {"123", "airplane", "gogogo"};
    for (int i = 0; i < 3; i++)
        printf("%s: %d\n", str[i], hash_blob(str[i], 3));
}