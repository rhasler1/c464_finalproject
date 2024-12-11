### Installation
__Dependencies:__
1. CMake: Minimum required version 3.28.
2. OpenMP: Most versions should work.

__Installation and Build Process:__
1. Clone or fork Repo: `git clone https://github.com/rhasler1/c464_finalproject.git`.
2. Change working directory: `cd c464_finalproject`
3. Make build directory: `mkdir build-release`
4. Change working directory: `cd build-release`
5. Generate build files: `cmake ..`
6. Build: `cmake --build .`

__Running Tests:__
1. Change directory to build-release and run: `./bin/tests`

__Executing code:__
1. Change directory to build-release and run: `./bin/floyd_warshall <args>`
- List of args:
    - -s: sequential mode of execution
    - -n: naive mode of execution
    - -b: blocked mode of execution (tiled)
    - -v: specify number of vertices
    - -e: specify number of edges
    - -p: print adj matrix before and after
    - -t: specify number of threads
    - -l: specify block length
    - -i: specify number of iterations to run

- Note there are dependencies/requirements on some of the args, e.g:,
    - Vertices must be divisible by block length.
    - A mode of execution must be specified.

Example exeuction 1: ./bin/floyd_warshall -s

Example exeuction 2: ./bin/floyd_warshall -b -l 20 -v 500 -e 500 -t 8


__Running benchmarks__
1. Strong scaling benchmark: `strong_scaling_test.sh`
2. Weak scaling benchmark: `weak_scaling_test.sh`

