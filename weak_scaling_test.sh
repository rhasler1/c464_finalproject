#!/bin/bash

# Note: Test for strong scaling.
# Number of vertices: 1k, 2k, 4k, 8k
# Number of threads: 1, 2, 4, 8, 16, 32
#
# 1. Run 1k w/ 1, 2, 4, 8, 16, and 32 threads.
# 2. Increment vertices.

# Variables
# Replace with path to executable.
EXECUTABLE="./build-testing-2/bin/floyd_warshall"						# Path to executable
OUTPUT_FILE="weak_b_results.txt"								# File to store the output

STEPS=5

MODE="-b"											# -s: sequential, -n: naive, -b: blocked

# Hard coding threads b/c vertices must be hard coded.
THREAD0=1												
THREAD1=2
THREAD2=4
THREAD3=8
THREAD4=16
THREAD5=32
# Hard coding the vertices sizes b/c the number of vertices must be divisible by block length.
VERTICE0=1000											# #Vertices for 1 thread.
VERTICE1=1200											# #Vertices for 2 threads.
VERTICE2=1600											# #Vertices for 4 threads.
VERTICE3=2000											# #Vertices for 8 threads.
VERTICE4=2600											# #Vertices for 16 threads.
VERTICE5=3000											# #Vertices for 32 threads.

EDGES=1000											# Number of edges
LENGTH=20											# Block length


# Clear the output file if it exists
>"$OUTPUT_FILE"

echo "Run with $VERTICE0 Vertices and $THREAD0 Threads" >> "$OUTPUT_FILE"
$EXECUTABLE $MODE -t $THREAD0 -e $EDGES -v $VERTICE0 -l $LENGTH -i $STEPS>> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "Run with $VERTICE1 Vertices and $THREAD1 Threads" >> "$OUTPUT_FILE"
$EXECUTABLE $MODE -t $THREAD1 -e $EDGES -v $VERTICE1 -l $LENGTH -i $STEPS>> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "Run with $VERTICE2 Vertices and $THREAD2 Threads" >> "$OUTPUT_FILE"
$EXECUTABLE $MODE -t $THREAD2 -e $EDGES -v $VERTICE2 -l $LENGTH -i $STEPS>> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "Run with $VERTICE3 Vertices and $THREAD3 Threads" >> "$OUTPUT_FILE"
$EXECUTABLE $MODE -t $THREAD3 -e $EDGES -v $VERTICE3 -l $LENGTH -i $STEPS>> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "Run with $VERTICE4 Vertices and $THREAD4 Threads" >> "$OUTPUT_FILE"
$EXECUTABLE $MODE -t $THREAD4 -e $EDGES -v $VERTICE4 -l $LENGTH -i $STEPS>> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

echo "Run with $VERTICE5 Vertices and $THREAD5 Threads" >> "$OUTPUT_FILE"
$EXECUTABLE $MODE -t $THREAD5 -e $EDGES -v $VERTICE5 -l $LENGTH -i $STEPS>> "$OUTPUT_FILE"
echo "" >> "$OUTPUT_FILE"

# Output message
echo "Finished running $EXECUTABLE $OUTTER_ITERS times, Output saved to $OUTPUT_FILE."
