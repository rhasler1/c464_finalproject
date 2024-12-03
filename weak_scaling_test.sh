#!/bin/bash

# Note: Test for strong scaling.
# Number of vertices: 1k, 2k, 4k, 8k
# Number of threads: 1, 2, 4, 8, 16, 32
#
# 1. Run 1k w/ 1, 2, 4, 8, 16, and 32 threads.
# 2. Increment vertices.

# Variables
EXECUTABLE="./build-testing-2/bin/floyd_warshall"		# Path to executable
OUTPUT_FILE="weak_n_results.txt"						# File to store the output
ITERS=5
STEPS=1

MODE="-n"												# -s: sequential, -n: naive, -b: blocked
THREADS=1												# Starting thread count
MAX_THREADS=16											# Maximum number of threads available on system.
# Hard coding the vertices sizes b/c the number of vertices must be divisible by block length.
VERTICE0=1000											# #Vertices for 1 thread.
VERTICE1=1200											# #Vertices for 2 threads.
VERTICE2=1600											# #Vertices for 4 threads.
VERTICE3=2000											# #Vertices for 8 threads.
VERTICE4=2200											# #Vertices for 11 threads.
EDGES=1000												# Number of edges
LENGTH=200												# Tile length


# Clear the output file if it exists
>"$OUTPUT_FILE"



#TODO: Currently benchmarks for strong scaling; change to benchmark for weak scaling.
# Loop to run the executable
for ((i=0; i < VERTICE_ITERS; i++)); do
	for ((j=0; j < THREAD_ITERS; j++)); do
		echo "Run with $VERTICES Vertices and $THREADS Threads" >> "$OUTPUT_FILE"
		$EXECUTABLE $MODE -t $THREADS -e $EDGES -v $VERTICES -l $LENGTH -i $STEPS>> "$OUTPUT_FILE"
		echo "" >> "$OUTPUT_FILE"

		if ((THREADS < MAX_THREADS)); then
			THREADS=$((THREADS * 2))
		else
			THREADS=1
		fi
	done
	VERTICES=$((VERTICES * 2))
done

# Output message
echo "Finished running $EXECUTABLE $OUTTER_ITERS times, Output saved to $OUTPUT_FILE."