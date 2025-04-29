#!/bin/bash

# Create results.txt if it doesn't exist
if [ ! -f "results.txt" ]; then
    touch results.txt
fi

# Run the command 30 times
for i in $(seq 1 30); do
    time=$(./pc | grep "time" | awk '{print $4}')
    echo "$time" >> results.txt
done
