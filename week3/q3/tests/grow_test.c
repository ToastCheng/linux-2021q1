#include "../xs.h"

int main(int argc, char *argv[])
{
    xs *string = xs_new(&xs_literal_empty(), "12345");
    printf("type should be short (0): %d, capcity: %ld\n", xs_type(string), xs_capacity(string));
    xs_grow(string, 16);
    printf("type should be medium (1): %d, capcity: %ld\n", xs_type(string), xs_capacity(string));
    xs_grow(string, 256);
    printf("type should be large (2): %d, capcity: %ld\n", xs_type(string), xs_capacity(string));

    return 0;
}