#!/bin/bash

count=$#

echo "Всего $count чисел"

for i in "$@"
do
    sum=$((sum + i))
done

avg=$((sum/count))

echo $avg