#!/usr/bin/env bash
# A and B are executables you want to compare, gen takes int
# as command line arg. Usage: './stress.sh' (bash, not sh)
for((i = 1; ; ++i)); do
    echo $i
    ./gen $i > in
    # Remember to compile!!!
    ./A < in > out1
    ./B < in > out2
    diff -w out1 out2 || break
    # diff -w <(./A < in) <(./B < in) || break
    # ./A < in > out || break
done