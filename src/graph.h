#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

/**
 * @brief Generates a random directed graph represented as an adjacency matrix in flattened form.
 * 
 * This function populates a flattened adjacency matrix with a randomly generated directed graph. 
 * The graph includes a specified number of vertices and edges, ensuring no self-loops or duplicate 
 * edges. The diagonal entries represent the distance from a vertex to itself (set to `0`), valid edges 
 * are assigned a weight of `1`, and disconnected vertices are represented as `INF`.
 * 
 * @param graph A reference to a vector representing the adjacency matrix in flattened form. 
 *              The function modifies this vector to represent the generated graph.
 * @param vertices The number of vertices in the graph. The adjacency matrix is assumed to be 
 *                 of size `vertices x vertices`.
 * @param edges The number of directed edges to generate in the graph.
 * @return int Returns `1` on successful generation, or `-1` if the requested number of edges 
 *             exceeds the maximum possible for the given number of vertices.
 * 
 * @details
 * - **Memory Initializer**: All diagonal entries are set to `0`, representing the distance 
 *   from a vertex to itself. All other entries are set to `INF`, representing no connection.
 * - **Edge Generation**: Randomly generates unique directed edges, discarding self-loops and 
 *   duplicate edges. The edges are assigned a weight of `1` in the adjacency matrix.
 * - **Edge Constraints**: Ensures that the number of requested edges does not exceed the maximum 
 *   possible edges for the given number of vertices (`vertices * (vertices - 1)` for a directed graph).
 * - **Error Handling**: Logs an error message using `spdlog` and returns `-1` if the edge count 
 *   exceeds the maximum possible.
 * 
 * @note 
 * - `INF` should be predefined in the program to represent no connection between vertices.
 * - The function does not currently handle edge weights greater than `1`. If weighted edges are 
 *   needed, the function must be modified.
 * 
 * @limitations
 * - The function currently uses `rand()` for randomness, which may not provide sufficient randomness 
 *   for all use cases. Consider replacing with `std::random_device` or `std::mt19937` for better 
 *   randomness if required.
 * - The adjacency matrix assumes a fixed weight of `1` for valid edges. Modify the weight logic 
 *   if varying weights are required.
 */
int generate_linear_graph(
    std::vector<int> & graph,
    int vertices,
    int edges
);

/**
 * @brief Prints the adjacency matrix of a graph with special handling for infinite values.
 * 
 * This function outputs the adjacency matrix representation of a graph to the console. 
 * Each element is displayed row by row, separated by a space, with rows printed on new lines. 
 * Infinite values (`INF`) are represented by the symbol `N` for improved readability.
 * 
 * @param graph A reference to a vector representing the adjacency matrix of the graph 
 *              in flattened form. Each element `graph[i * vertices + j]` represents 
 *              the weight of the edge from vertex `i` to vertex `j`.
 * @param vertices The number of vertices in the graph. The adjacency matrix is assumed 
 *                 to be of size `vertices x vertices`.
 * 
 * @details
 * - The graph is stored as a flattened vector, and this function calculates the 2D index 
 *   `(i, j)` using the formula `i * vertices + j`.
 * - If the value of `graph[i * vertices + j]` is `INF`, it is printed as `N` to denote 
 *   no connection between the vertices.
 * - All other values are printed as integers, separated by a space for clarity.
 * 
 * @note
 * - The graph values should be properly initialized before calling this function.
 * - Ensure that `INF` is consistently defined in the program to avoid misinterpretation.
 */
void print_graph(
    std::vector<int> & graph,
    int vertices
);

#endif