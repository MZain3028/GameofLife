#!/bin/bash

# Compile the C program
gcc -pthread -o game_of_life game_of_life.c

# Define the grid size and number of generations
GRID_SIZE=100
GENERATIONS=10

# Define the number of threads/processes to test
NUM_PROCESSES=(1 2 4 8)

# Create a data file to store the execution times
data_file="execution_times.txt"
rm -f "$data_file"
touch "$data_file"

# Run the program with different number of threads/processes and measure the execution time
for num_proc in "${NUM_PROCESSES[@]}"; do
    echo "Running with $num_proc processes..."
    execution_time=$(time -f "%e" ./game_of_life $GRID_SIZE $GENERATIONS $num_proc 2>&1)
    echo "$num_proc $execution_time" >> "$data_file"
done
