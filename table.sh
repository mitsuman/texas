#!/bin/bash

mkdir -p dat

for ((i=2;i<=9;i++)); do
    s="./sim.exe $i 40000000 dat/hole-n$i-l1.dat"
    echo $s
    $s
done
