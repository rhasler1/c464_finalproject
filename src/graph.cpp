// Inspired by: https://www.geeksforgeeks.org/how-to-create-a-random-graph-in-c/
// Why to linearize: https://community.khronos.org/t/matrix-multiplication-using-2d-std-vector/106457#:~:text=On%20a%20more,be%20known%20statically.

#include "graph.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

int generate_simple_directed_linear(std::vector<int> & graph, int vertices, int edges)
{
    if (edges > vertices * (vertices - 1)) {
        spdlog::error("Number of edges {} exceeds what is possible given number of vertices {}", edges, vertices);
        return -1;
    }
    
    std::vector<std::vector<int>> edge(edges, std::vector<int>(2));
    int i = 0;
    
    // Generate directed graph.
    while (i < edges) {
        edge[i][0] = rand() % vertices;
        edge[i][1] = rand() % vertices;
        
        // Discard loop.
        if (edge[i][0] == edge[i][1]) {
            continue;
        }
        // Discard duplicate edge.
        else {
            for (int j = 0; j < i; j++) {
                if ((
                    edge[i][0] == edge[j][0] &&
                    edge[i][1] == edge[j][1]
                    ))
                {
                    i--;
                }
            }
        }
        i++;
    }
    
    // Initialize linear adjacency matrix.
    int vertex_one, vertex_two;
    for (int j = 0; j < edges; j++) {
        vertex_one = edge[j][0];
        vertex_two = edge[j][1];
        graph[vertex_one * vertices + vertex_two] = 1;
    }
    
    return 1;
}