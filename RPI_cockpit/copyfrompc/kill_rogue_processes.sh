#!/bin/bash


procs=$(ps -ef | grep cockpit)

while IFS= read -r line; do
    echo "$line"
    b=$( echo "$line" | awk ' { print $2 } ')
    echo "killing $b"
    kill "$b"
    echo "result = $?"
done < <(echo "$procs" | grep _main )

