#include "graph.h"
#include "plf_nanotimer.h"
#include "timestamps.h"
#include <omp.h>
#include <CLI/CLI.hpp>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

using namespace std;

void blocked_floyd_warshall(vector<int> &W, int n, int b);
inline int block_idx(int i, int j, int b);
void floyd(vector<int> &C, const vector<int> &A, const vector<int> &B, int b);



int main(const int argc, const char *const argv[])
{
    // Initializing defaults.
    bool run_sequential{false};                                 // Option to run sequential code.
    bool run_naive_parallel{false};                             // Option to run naive parallel code.
    bool run_block_parallel{false};                             // Option to run cache optimized parallel code.
    bool print{false};                                          // Option to print to console.

    int vertices{100};                                         // Default to 100 nodes.
    int edges{200};                                            // Default to 200 edges.
    int threads{1};                                            // Default to 1 thread.
    int tile_length{1};                                        // Default to 1 length.

    double time_result;                                         // Variable used to mark time.
    std::vector<std::tuple<std::string, double>> timestamps;    // Place to store timestamps.


    // CLI setup and parse.
    CLI::App app{"Floyd-Warshall"};
    app.option_defaults()->always_capture_default(true);
    app.add_option("-v, --vertices", vertices)
        ->check(CLI::PositiveNumber.description(" >= 1"));      // Change to >= 0; look at CLI API.
    app.add_option("-e, --edges", edges)
        ->check(CLI::PositiveNumber.description(" >= 1"));      // Change to >= 0; look at CLI API.
    app.add_option("-t, --threads", threads)
        ->check(CLI::PositiveNumber.description(" >= 1"));
    app.add_option("-l, --tile-length", tile_length)
        ->check(CLI::PositiveNumber.description(" >= 1"));
    app.add_flag("-s, --sequential", run_sequential);
    app.add_flag("-n, --naive-parallel", run_naive_parallel);
    app.add_flag("-p, --block-parallel", run_block_parallel);
    CLI11_PARSE(app, argc, argv);

    // Log the number of vertices and edges.
    spdlog::info("Number of vertices: {}", vertices);
    spdlog::info("Number of edges: {}", edges);

    // Ensure tile length is smaller than graph length.
    if (tile_length > vertices) {
        spdlog::error("Tile length {} cannot be greater than number of vertices {}",
        tile_length,
        vertices
        );
    }

    // Ensure threads is within available range.
    // No need to check the minimum, this is taken care of during CLI parse
    // (see: ->check(CLI::PositiveNumber.description(" >= 1")))
    int max_threads = omp_get_max_threads();
    if (threads > max_threads)
    {
        spdlog::info("Argument threads {} is greater than max threads {}", threads, max_threads);
        spdlog::info("Setting threads to max threads...");
        threads = max_threads;
        spdlog::info("Threads is now {}", threads);
    }
    omp_set_num_threads(threads);

    // Testing if threads were properly set.
    #pragma omp parallel for
    for (int t = 0; t < 1; t++) {
        fmt::print("Number of threads being used: {}", omp_get_num_threads());
    }

    // Ensure user input at least one mode of execution.
    if (
        !run_sequential &&
        !run_naive_parallel &&
        !run_block_parallel
        )
        {
            spdlog::error(
                "\n"
                "Specify mode of execution: \n"
                "-s: sequential \n"
                "-n: naive-parallel (No cache optimizations) \n"
                "-p: block-parallel (Cache optimizations) \n"
                );
            return 1;
        }
    
    // Generate graph.
    spdlog::info("Generating graph data...");
    std::vector<int> graph(vertices * vertices);               // Adjacency matrix, see graph.cpp for details.
    if (!generate_linear_graph(graph, vertices, edges)) {
        spdlog::error("Failed to generate graph... Exiting program.");
        return 1;
    }
    spdlog::info("Done populating graph with data.");

    if (run_sequential)
    {
        spdlog::info("Beginning Floyd-Warshall sequential execution...");
        spdlog::info("Starting nanotimer...");
        plf::nanotimer sequential_time;
        sequential_time.start();
        for (int k = 0; k < vertices; k++) {
            for (int i = 0; i < vertices; i++) {
                for (int j = 0; j < vertices; j++) {
                    if (graph[i * vertices + j] > (graph[i * vertices + k] + graph[k * vertices + j]))
                    {
                        // Note: I am assuming weights are not INF.
                        graph[i * vertices + j] = graph[i * vertices + k] + graph[k * vertices + j];
                    }
                }
            }
        }
        time_result = sequential_time.get_elapsed_ns();
        spdlog::info("Sequential execution done.");
        spdlog::info("Getting elapsed time...");
        mark_time(timestamps, time_result, "Sequential time");
    }

    else if (run_naive_parallel)
    {
        spdlog::info("Beginning Floyd-Warshall parallel without cache optimizations");
        spdlog::info("Beginning nanotimer...");
        plf::nanotimer naive_parallel_time;
        naive_parallel_time.start();
        for (int k = 0; k < vertices; k++) {
            // Collapse the i and j loops into a single iteration space;
            // this allows both loops to parallelize together.
            #pragma omp parallel for collapse(2) schedule(static)
            for (int i = 0; i < vertices; i++) {
                for (int j = 0; j < vertices; j++) {
                    if (graph[i * vertices + j] > (graph[i * vertices + k] + graph[k * vertices + j]))
                    {
                        // Note: I am assuming weights are not INF.
                        graph[i * vertices + j] = graph[i * vertices + k] + graph[k * vertices + j];
                    }
                }
            }
        }
        time_result = naive_parallel_time.get_elapsed_ns();
        spdlog::info("Naive execution done.");
        spdlog::info("Getting elapsed time...");
        mark_time(timestamps, time_result, "Naive time");
    }

    else if (run_block_parallel)
    {
        spdlog::info("Beginning Floyd-Warshall parallel with cache optimizations");
        spdlog::info("Beginning nanotimer...");
        plf::nanotimer optimized_parallel_time;
        optimized_parallel_time.start();
        blocked_floyd_warshall(graph, vertices, tile_length);
        time_result = optimized_parallel_time.get_elapsed_ns();
        spdlog::info("Optimized execution done.");
        spdlog::info("Getting elapsed time...");
        mark_time(timestamps, time_result, "Optimized time");
    }

    // Printing execution details.
    spdlog::info("Printing graph details...");
    fmt::print(
        "Number of vertices: {}\nNumber of edges: {}\nGraph memory footprint: {}\n",
        vertices,
        edges,
        graph.capacity() * sizeof(graph[0])
    );
    spdlog::info("Printing timestamps...");
    print_timestamps(timestamps);
    spdlog::info("Exiting program.");
    return 0;
}


// Helper function to calculate 1D index in a block
inline int block_idx(int i, int j, int b) {
    return i * b + j;
}

// Floyd-Warshall on a single block (b × b matrix stored as a 1D vector)
void floyd(vector<int> &C, const vector<int> &A, const vector<int> &B, int b) {
    for (int k = 0; k < b; ++k) {
        for (int j = 0; j < b; ++j) {
            for (int i = 0; i < b; ++i) {
                int c_idx = block_idx(i, j, b);
                int a_idx = block_idx(i, k, b);
                int b_idx = block_idx(k, j, b);
                C[c_idx] = min(C[c_idx], A[a_idx] + B[b_idx]);
            }
        }
    }
}

// Blocked Floyd-Warshall
void blocked_floyd_warshall(vector<int> &W, int n, int b) {
    int B = n / b; // Number of blocks along one dimension

    // Iterate over all block rows and columns
    for (int k = 0; k < B; ++k) {
        // Dependent Phase: Process block W[k][k]
        vector<int> Wkk(b * b);
        for (int i = 0; i < b; ++i) {
            for (int j = 0; j < b; ++j) {
                Wkk[block_idx(i, j, b)] = W[block_idx(k * b + i, k * b + j, n)];
            }
        }
        floyd(Wkk, Wkk, Wkk, b);

        // Write back W[k][k]
        for (int i = 0; i < b; ++i) {
            for (int j = 0; j < b; ++j) {
                W[block_idx(k * b + i, k * b + j, n)] = Wkk[block_idx(i, j, b)];
            }
        }

        // Partially Dependent Phase: Update rows and columns around W[k][k]
        #pragma omp parallel for
        for (int j = 0; j < B; ++j) {
            if (j != k) {
                vector<int> Wkj(b * b), Wkj_tmp(b * b);
                for (int i = 0; i < b; ++i) {
                    for (int l = 0; l < b; ++l) {
                        Wkj[block_idx(i, l, b)] = W[block_idx(k * b + i, j * b + l, n)];
                        Wkj_tmp[block_idx(i, l, b)] = Wkj[block_idx(i, l, b)];
                    }
                }
                floyd(Wkj_tmp, Wkk, Wkj, b);
                for (int i = 0; i < b; ++i) {
                    for (int l = 0; l < b; ++l) {
                        W[block_idx(k * b + i, j * b + l, n)] = Wkj_tmp[block_idx(i, l, b)];
                    }
                }
            }
        }

        #pragma omp parallel for
        for (int i = 0; i < B; ++i) {
            if (i != k) {
                vector<int> Wik(b * b), Wik_tmp(b * b);
                for (int j = 0; j < b; ++j) {
                    for (int l = 0; l < b; ++l) {
                        Wik[block_idx(j, l, b)] = W[block_idx(i * b + j, k * b + l, n)];
                        Wik_tmp[block_idx(j, l, b)] = Wik[block_idx(j, l, b)];
                    }
                }
                floyd(Wik_tmp, Wik, Wkk, b);
                for (int j = 0; j < b; ++j) {
                    for (int l = 0; l < b; ++l) {
                        W[block_idx(i * b + j, k * b + l, n)] = Wik_tmp[block_idx(j, l, b)];
                    }
                }
            }
        }

        // Independent Phase: Update all other blocks
        #pragma omp parallel for
        for (int i = 0; i < B; ++i) {
            if (i != k) {
                for (int j = 0; j < B; ++j) {
                    if (j != k) {
                        vector<int> Wij(b * b), Wik(b * b), Wkj(b * b);
                        for (int x = 0; x < b; ++x) {
                            for (int y = 0; y < b; ++y) {
                                Wij[block_idx(x, y, b)] = W[block_idx(i * b + x, j * b + y, n)];
                                Wik[block_idx(x, y, b)] = W[block_idx(i * b + x, k * b + y, n)];
                                Wkj[block_idx(x, y, b)] = W[block_idx(k * b + x, j * b + y, n)];
                            }
                        }
                        floyd(Wij, Wik, Wkj, b);
                        for (int x = 0; x < b; ++x) {
                            for (int y = 0; y < b; ++y) {
                                W[block_idx(i * b + x, j * b + y, n)] = Wij[block_idx(x, y, b)];
                            }
                        }
                    }
                }
            }
        }
    }
}

// KERNEL CODE
    /*
                for (j = 0; j < V; j++) {
                // If vertex k is on the shortest path from
                // i to j, then update the value of
                // dist[i][j]
                if (dist[i][j] > (dist[i][k] + dist[k][j])
                    && (dist[k][j] != INF
                        && dist[i][k] != INF))
                    dist[i][j] = dist[i][k] + dist[k][j];
            }
    
    */


/*
    else if (run_block_parallel)
    {
        spdlog::info("Beginning Floyd-Warshall parallel with cache optimizations");
        spdlog::info("Beginning nanotimer...");
        plf::nanotimer optimized_parallel_time;
        optimized_parallel_time.start();
        for (int k = 0; k < vertices; k++) {
            // TODO: algorithm needs work.
            // Process tiles
            #pragma omp parallel for collapse(2) schedule(static)
            for (int i_tile = 0; i_tile < vertices; i_tile += tile_length) {
                for (int j_tile = 0; j_tile < vertices; j_tile += tile_length) {
                    // Process elements within the current tile
                    int i_tile_end = (i_tile + tile_length < vertices) ? i_tile + tile_length : vertices;
                    int j_tile_end = (j_tile + tile_length < vertices) ? j_tile + tile_length : vertices;
                    for (int i = i_tile; i < i_tile_end; i++) {
                        for (int j = j_tile; j < j_tile_end; j++) {
                            // Update graph with the current intermediate vertex k
                            if (graph[i * vertices + j] > graph[i * vertices + k] + graph[k * vertices + j])
                            {
                                graph[i * vertices + j] = graph[i * vertices + k] + graph[k * vertices + j];
                            }
                        }
                    }
                }
            }
        }
        time_result = optimized_parallel_time.get_elapsed_ns();
        spdlog::info("Optimized execution done.");
        spdlog::info("Getting elapsed time...");
        mark_time(timestamps, time_result, "Optimized time");
    }
*/