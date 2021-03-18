#include "../xs.h"

int main(int argc, char *argv[])
{
    xs string = *xs_new(&xs_literal_empty(), "12345678901234567890");

    xs prefix = *xs_tmp("((("), suffix = *xs_tmp(")))");
    xs_concat(&string, &prefix, &suffix);
    printf("[%s] : %2zu\n", xs_data(&string), xs_size(&string));
    return 0;
}