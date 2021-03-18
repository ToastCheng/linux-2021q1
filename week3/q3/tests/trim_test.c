#include "../xs.h"

int main(int argc, char *argv[])
{
    xs string = *xs_new(&xs_literal_empty(), "\n 12345678901234567890 \n\n\n");
    xs_trim(&string, "\n ");
    printf("[%s] : %2zu\n", xs_data(&string), xs_size(&string));

    return 0;
}