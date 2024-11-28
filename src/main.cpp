#include "graph.h"
#include "plf_nanotimer.h"
#include "timestamps.h"
#include "kernels.h"
#include "globals.h"
#include <omp.h>
#include <CLI/CLI.hpp>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

int main(const int argc, const char *const argv[])
{
    // Initializing defaults.
    bool run_sequential{false};
    bool run_naive_parallel{false};
    bool run_block_parallel{false};
    bool print{false};

    int vertices{100};
    int edges{200};
    int threads{1};
    int block_length{1};

    double time_result;
    std::vector<std::tuple<std::string, double>> timestamps;

    // CLI setup and parse.
    CLI::App app{"Floyd-Warshall"};
    app.option_defaults()->always_capture_default(true);
    app.add_option("-v, --vertices", vertices)
        ->check(CLI::PositiveNumber.description(" >= 1"));
    app.add_option("-e, --edges", edges)
        ->check(CLI::PositiveNumber.description(" >= 1"));
    app.add_option("-t, --threads", threads)
        ->check(CLI::PositiveNumber.description(" >= 1"));
    app.add_option("-l, --block-length", block_length)
        ->check(CLI::PositiveNumber.description(" >= 1"));
    app.add_flag("-s, --sequential", run_sequential);
    app.add_flag("-n, --naive-parallel", run_naive_parallel);
    app.add_flag("-b, --block-parallel", run_block_parallel);
    app.add_flag("-p, --print", print);
    CLI11_PARSE(app, argc, argv);

    // Log the number of vertices and edges.
    //spdlog::info("Number of vertices: {}", vertices);
    //spdlog::info("Number of edges: {}", edges);

    // Check if block length is greater than number of vertices.
    if (block_length > vertices)
    {
        spdlog::error(
            "Block length {} cannot be greater than number of vertices {}",
            block_length,
            vertices
        );
        return 1;
    }

    // Check if the number of vertices is divisible by block length.
    if (vertices % block_length != 0)
    {
        spdlog::error(
            "Vertices: {} must be divisible by block length: {}",
            vertices,
            block_length
        );
        return 1;
    }

    // Check if user input for threads is within maximum. If greater than, set to maximum.
    int max_threads = omp_get_max_threads();
    if (threads > max_threads)
    {
        spdlog::info(
            "Argument threads {} is greater than max threads {}",
            threads,
            max_threads
        );
        spdlog::info("Setting threads to max threads...");
        threads = max_threads;
        spdlog::info("Threads is now {}", threads);
    }
    omp_set_num_threads(threads);

    // Print number of OMP threads in use -- used for testing.
    //#pragma omp parallel for
    //for (int t = 0; t < 1; t++) {
    //    fmt::print(
    //        "Number of threads being used: {}\n",
    //        omp_get_num_threads()
    //    );
    //}

    // Check user input for mode of execution.
    if
    (
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
            "-b: block-parallel (Cache optimizations) \n"
        );
        return 1;
    }
    
    // Generate graph.
    spdlog::info("Generating graph data...");
    std::vector<int> graph(vertices * vertices, INF);
    if (generate_linear_graph(graph, vertices, edges) == -1)
    {
        spdlog::error("Failed to generate graph... Exiting program.");
        return 1;
    }
    spdlog::info("Done populating graph with data.");

    // Print generated graph.
    if (print)
    {
        fmt::print("Graph before Floyd-Warshall:\n");
        print_graph(graph, vertices);
    }

    if (run_sequential)
    {
        spdlog::info("Beginning Floyd-Warshall sequential execution...");
        spdlog::info("Starting nanotimer...");
        plf::nanotimer sequential_time;
        sequential_time.start();
        serial_floyd_warshall(graph, vertices);
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
        naive_floyd_warshall(graph, vertices);
        time_result = naive_parallel_time.get_elapsed_ns();
        spdlog::info("Naive execution done.");
        spdlog::info("Getting elapsed time...");
        mark_time(timestamps, time_result, "Naive time");
    }

    else if (run_block_parallel)
    {
        spdlog::info("Beginning Floyd-Warshall parallel with cache optimizations");
        spdlog::info("Beginning nanotimer...");
        plf::nanotimer block_parallel_time;
        block_parallel_time.start();
        blocked_floyd_warshall(graph, vertices, block_length);
        time_result = block_parallel_time.get_elapsed_ns();
        spdlog::info("Optimized execution done.");
        spdlog::info("Getting elapsed time...");
        mark_time(timestamps, time_result, "Block time");
    }

    // Print execution details.
    spdlog::info("Printing graph details...");
    if (print)
    {
        fmt::print("Graph after Floyd-Warshall:\n");
        print_graph(graph, vertices);
    }
    fmt::print(
        "Number of vertices: {}\nNumber of edges: {}\nGraph memory footprint: {}\nNumber of threads: {}\nBlock length: {}\n",
        vertices,
        edges,
        graph.capacity() * sizeof(graph[0]),
        threads,
        block_length
    );
    spdlog::info("Printing timestamps...");
    print_timestamps(timestamps);
    spdlog::info("Exiting program.");
    return 0;
}