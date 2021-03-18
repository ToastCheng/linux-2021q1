typedef struct {
    int x;
} s;

int main() {
    __typeof__(((s *) 5566)->x) a = 1;
    return 0;
}
