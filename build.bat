@echo off

mkdir bin
clang -O3 -Wall -o bin\test.exe -Iinclude src\*.c
