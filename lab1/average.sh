#!/bin/bash
count=$#
sum=0
for i in "$@"
do
    sum=$((sum + i))
done
avg=$((sum/count))
echo "Всего $count чисел"
echo "Среднее: $avg"
