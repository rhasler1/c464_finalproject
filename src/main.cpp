#include "graph.h"
#include "plf_nanotimer.h"
#include "timestamps.h"
#include <omp.h>
#include <CLI/CLI.hpp>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>


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
        std::vector<int> tile(tile_length * tile_length);       // Temporary storage for a tile.       
        for (int k = 0; k < vertices; k++) {
            // TODO: algorithm needs work.
            // Process tiles
            #pragma omp parallel for collapse(2) schedule(static)
            for (int i_tile = 0; i_tile < vertices; i_tile += tile_length) {
                for (int j_tile = 0; j_tile < vertices; j_tile += tile_length) {
                    int i_tile_end = std::min(i_tile + tile_length, vertices);
                    int j_tile_end = std::min(j_tile + tile_length, vertices);

                    // Copy the tile into temporary storage.
                    //#pragma omp parallel for collapse(2) schedule(static)
                    for (int i = i_tile; i < i_tile_end; i++) {
                        for (int j = j_tile; j < j_tile_end; j++) {
                            tile[(i - i_tile) * tile_length + (j - j_tile)] = graph[i * vertices + j];
                        }
                    }

                    // Perform computation on tile.
                    // Perform computation on the tile
                    //#pragma omp parallel for collapse(2) schedule(static)
                    for (int i = i_tile; i < i_tile_end; i++) {
                        for (int j = j_tile; j < j_tile_end; j++) {
                            int tile_i = i - i_tile;
                            int tile_j = j - j_tile;
                            if (tile[tile_i * tile_length + tile_j] > graph[i * vertices + k] + graph[k * vertices + j]) {
                                tile[tile_i * tile_length + tile_j] = graph[i * vertices + k] + graph[k * vertices + j];
                            }
                        }
                    }
                    // Copy the tile back into the graph
                    //#pragma omp parallel for collapse(2) schedule(static)
                    for (int i = i_tile; i < i_tile_end; i++) {
                        for (int j = j_tile; j < j_tile_end; j++) {
                            graph[i * vertices + j] = tile[(i - i_tile) * tile_length + (j - j_tile)];
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