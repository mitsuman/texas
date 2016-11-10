#!/bin/bash

mkdir -p html
rm -f html/random.html

OUT=html/random.html

echo "<html><body>\n<pre style='color:#ccc;background:#000;padding:4px;font:12px \"Lucida Console\"'>" > $OUT

for ((i=2;i<=9;i++)); do
    s="./sim.exe $i 20000000 - h"
    echo $s
    $s >> $OUT
done

echo "</pre></body></html>" >> $OUT
