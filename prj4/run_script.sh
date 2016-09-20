#!/bin/bash

echo "Default Configuration"

for i in `ls ./traces/`; do
	echo ""
	echo "--$i--"
		./cachesim -i ./traces/$i
done;

echo ""
echo "Direct Mapped Cache"
echo "C = 10, S = 0"

for i in `ls ./traces/`; do
	echo ""
	echo "--$i--"
	./cachesim -c 10 -s 0 -i ./traces/$i
done;

echo ""
echo "4 Way Associative"
echo "C = 12, S = 2, B = 4"

for i in `ls ./traces/`; do
	echo ""
	echo "--$i--"
	./cachesim -c 12 -s 2 -b 4 -i ./traces/$i
done;

echo ""
echo "Fully Associative"
echo "C = 12, S = 8, B = 4"

for i in `ls ./traces/`; do
	echo ""
	echo "--$i--"
	./cachesim -c 12 -s 8 -b 4 -i ./traces/$i
done;
