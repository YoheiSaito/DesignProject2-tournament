#!/bin/bash
for i in `seq 1 ${2}`
do
	./gen-rand-evl ../evl/${1}-${i}.evl
	echo "../evl/${1}-${i}.evl"
done
