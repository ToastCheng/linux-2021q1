// #include <stdio.h>

typedef struct {
    int x;
} s;

int main() {
    // int g = 1;
    // g++;
    __typeof__(((s *) 0)->x) a = 1;
    // printf("%d\n", a);
    return 0;
}

/*
0000000000001129 <main>:
    1129:	f3 0f 1e fa          	endbr64
    112d:	55                   	push   %rbp
    112e:	48 89 e5             	mov    %rsp,%rbp
    1131:	c7 45 fc 01 00 00 00 	movl   $0x1,-0x4(%rbp)
    1138:	b8 00 00 00 00       	mov    $0x0,%eax
    113d:	5d                   	pop    %rbp
    113e:	c3                   	retq
    113f:	90                   	nop
*/