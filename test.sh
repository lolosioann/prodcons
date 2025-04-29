#!/bin/bash

# Create results.txt if it doesn't exist
if [ ! -f "results.txt" ]; then
    touch results.txt
fi

# Clear the contents of results.txt
> results.txt
# Compile the C program
make clean
make
# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed."
    exit 1
fi
# Check if the compiled program exists
if [ ! -f "pc" ]; then
    echo "Compiled program not found."
    exit 1
fi

# Run the command 30 times
for i in $(seq 1 30); do
    time=$(./pc | grep "time" | awk '{print $4}')
    echo "$time" >> results.txt
done
