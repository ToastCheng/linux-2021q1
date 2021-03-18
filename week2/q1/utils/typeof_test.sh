#!/bin/bash

gcc -o t1 -O0 typeof_test_1.c
gcc -o t2 -O0 typeof_test_2.c
objdump -d ./t1 > o1
objdump -d ./t2 > o2
diff o1 o2