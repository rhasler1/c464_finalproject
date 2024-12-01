#!/bin/bash

# Note: Test for strong scaling.
# Number of vertices: 1k, 2k, 4k, 8k
# Number of threads: 1, 2, 4, 8, 16, 32
#
# 1. Run 1k w/ 1, 2, 4, 8, 16, and 32 threads.
# 2. Increment vertices.

# Variables
EXECUTABLE="./build-testing-2/bin/floyd_warshall"		# Path to executable
OUTPUT_FILE="results.txt"					# File to store the output
OUTTER_ITERS=4							# Vertices are incremented in outter loop. 
INNER_ITERS=6							# Threads are incremented in inner loop.

MODE="-n"							# -s: sequential, -n: naive, -b: blocked
THREADS=1							# Starting thread count
MAX_THREADS=32							# Maximum number of threads available on system.
VERTICES=1000							# Starting number of vertices
MAX_VERTICES=8000						# Maximum number of vertices
EDGES=500							# Number of edges
LENGTH=200							# Tile length


# Clear the output file if it exists
>"$OUTPUT_FILE"

# Loop to run the executable
for ((i=0; i < OUTTER_ITERS; i++)); do
	for ((j=0; j < INNER_ITERS; j++)); do
		echo "Run with $VERTICES Vertices and $THREADS Threads" >> "$OUTPUT_FILE"
		$EXECUTABLE $MODE -t $THREADS -e $EDGES -v $VERTICES -l $LENGTH>> "$OUTPUT_FILE"
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
