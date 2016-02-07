#!/bin/sh

mkdir -p bin
#gcc -g -Wall -o bin/test -Iinclude src/gc.c src/main.c
#gcc -O3 -Wall -o bin/test -Iinclude src/gc.c src/main.c
gcc -g -Wall -o bin/gc -Iinclude -I/usr/include/SDL src/*.c -lSDL
