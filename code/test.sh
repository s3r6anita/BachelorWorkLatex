#!/bin/bash

READER_ADDRESS=$1
MODE=$2
TRIES=$3

for ((i=1; i<=$TRIES; i++))
do
    echo "[Test $i] Тестирование $MODE"
    python3 hw_tester.py --reader $READER_ADDRESS --mode $MODE
done

echo "Тестирование завершено."