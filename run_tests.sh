#!/bin/bash

flex scanner.lex

g++ -std=c++17 lex.yy.c hw1.cpp -o hw1.out

for i in {1..10}; do
    ./hw1.out < tests/t${i}.in > tests/t${i}.in.res
    diff tests/t${i}.in.res tests/t${i}.in.out
done

echo "Done, if you don't see anything else on the screen then you probably did a good job!"