#!/bin/sh
gcc -I ../src ../src/slab.c performance_testcode_no_slab.c -o performance_no_slab
gcc -I ../src ../src/slab.c performance_testcode.c -o performance_slab
gcc -I ../src ../src/slab.c function_testcode.c -o function_slab
gcc -I ../src ../src/slab.c generater.c -o generate