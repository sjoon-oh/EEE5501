#!/bin/bash

make clean
make

for threads in 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024
do
	echo "Testing thread: ${threads}"
	./thread ${threads} data
	echo ""
done


