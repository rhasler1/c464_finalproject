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
	bool run_sequential{false};									// Option to run sequential code.
	bool run_naive_parallel{false};								// Option to run naive parallel code.
	bool run_block_parallel{false};								// Option to run cache optimized parallel code.
	bool print{false};											// Option to print to console.

	uint vertices{100};											// Default to 100 nodes.
	uint edges{200};											// Default to 200 edges.
	uint threads{1};											// Default to 1 thread.

	double time_result;											// Variable used to mark time.
	std::vector<std::tuple<std::string, double>> timestamps;	// Place to store timestamps.


	// CLI setup and parse.
	CLI::App app{"Floyd-Warshall"};
	app.option_defaults()->always_capture_default(true);
	app.add_option("-v, --vertices", vertices)
		->check(CLI::PositiveNumber.description(" >= 1"));		// Change to >= 0; look at CLI API.
	app.add_option("-e, --edges", edges)
		->check(CLI::PositiveNumber.description(" >= 1"));		// Change to >= 0; look at CLI API.
	app.add_option("-t, --threads", threads)
		->check(CLI::PositiveNumber.description(" >= 1"));
	app.add_flag("-s, --sequential", run_sequential);
	app.add_flag("-n, --naive-parallel", run_naive_parallel);
	app.add_flag("-p, --block-parallel", run_block_parallel);
	CLI11_PARSE(app, argc, argv);

	// Log the number of vertices and edges
	spdlog::info("Number of vertices: {}", vertices);
	spdlog::info("Number of edges: {}", edges);

	// Ensure threads is within available range.
	// No need to check the minimum, this is taken care of during CLI parse
	// (see: ->check(CLI::PositiveNumber.description(" >= 1")))
    uint max_threads = omp_get_max_threads();
	if (threads > max_threads)
	{
		spdlog::info("Argument threads {} is greater than max threads {}", threads, max_threads);
		spdlog::info("Setting threads to max threads...");
		threads = max_threads;
		spdlog::info("Threads is now {}", threads);
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
	std::vector<uint> graph(vertices * vertices);					// Adjacency matrix, see graph.cpp for details.
	if (!generate_linear_graph) {
		spdlog::error("Failed to generate graph... Exiting program.");
		return 1;
	}

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
	}

	else if (run_block_parallel)
	{
		//TODO
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
