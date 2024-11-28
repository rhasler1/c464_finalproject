#ifndef KERNEL_H
#define KERNEL_H

#include <vector>

/**
 * @brief Performs the blocked version of the Floyd-Warshall algorithm to compute all-pairs shortest paths.
 * Inspired by: https://www.researchgate.net/publication/350236410_Towards_performance_improvement_of_a_parallel_Floyd-Warshall_algorithm_using_OpenMP_and_Intel_TBB
 * 
 * This function divides the input adjacency matrix into smaller blocks to improve cache locality and computational efficiency.
 * It processes the matrix in phases: computing paths within a block (dependent phase), updating surrounding rows and columns
 * (partially dependent phase), and updating all other blocks (independent phase). It employs parallelization for improved performance.
 * 
 * @param W A reference to a vector representing the adjacency matrix of the graph in flattened form. The matrix is updated in-place.
 * @param n The dimension (number of vertices) of the adjacency matrix. The matrix is assumed to be `n x n`.
 * @param b The size of the blocks into which the adjacency matrix is divided. Must be a divisor of `n`.
 * 
 * The algorithm is organized into phases:
 * - **Dependent Phase**: Computes shortest paths within the diagonal block `W[k][k]`.
 * - **Partially Dependent Phase**: Updates the rows and columns around `W[k][k]`.
 * - **Independent Phase**: Updates all other blocks using the results from the previous phases.
 * 
 * The function uses helper functions `block_idx` to calculate indices in flattened blocks and `floyd` to compute shortest paths within a block.
 * OpenMP directives are used to parallelize certain phases for improved performance.
 * 
 * @note The input matrix `W` must be flattened and should have dimensions that are divisible by `b` to ensure proper block alignment.
 */
void blocked_floyd_warshall(
    std::vector<int> & W,
    int n,
    int b
);

/**
 * @brief Computes all-pairs shortest paths using the naive Floyd-Warshall algorithm.
 * Inspired by: https://www.geeksforgeeks.org/floyd-warshall-algorithm-dp-16/
 * 
 * This function implements the Floyd-Warshall algorithm to calculate the shortest path 
 * between all pairs of vertices in a graph. It uses a naive approach where the three 
 * nested loops iterate over all vertices to update the graph matrix in-place. The function 
 * is parallelized using OpenMP to improve performance.
 * 
 * @param graph A reference to a vector representing the adjacency matrix of the graph 
 *              in flattened form. The graph is updated in-place with the shortest path
 *              distances. Infinity (`INF`) is used to indicate the absence of an edge.
 * @param vertices The number of vertices in the graph. The adjacency matrix is assumed
 *                 to be of size `vertices x vertices`.
 * 
 * @details
 * The algorithm considers each vertex `k` as an intermediate vertex and iterates over 
 * all pairs of vertices `(i, j)`. If the path through `k` offers a shorter distance, the
 * direct path between `i` and `j` is updated. 
 * 
 * Key optimizations:
 * - The nested `i` and `j` loops are collapsed using OpenMP's `#pragma omp parallel for collapse(2)` 
 *   directive, enabling efficient parallel execution of the outer two loops.
 * - The addition operation is performed only if the paths through `k` are finite, ensuring 
 *   correctness when dealing with disconnected nodes.
 * 
 * @note The input graph must be initialized with appropriate weights, where:
 * - `graph[i * vertices + j]` is the weight of the edge from vertex `i` to vertex `j`.
 * - `INF` represents no direct edge between the vertices.
 */
void naive_floyd_warshall(
    std::vector<int> & graph,
    int vertices
);

/**
 * @brief Computes all-pairs shortest paths using the serial Floyd-Warshall algorithm.
 * 
 * This function implements the Floyd-Warshall algorithm in a serial manner to calculate 
 * the shortest paths between all pairs of vertices in a graph. It iteratively updates the 
 * adjacency matrix to reflect the shortest path distances, considering each vertex as an 
 * intermediate point.
 * 
 * @param graph A reference to a vector representing the adjacency matrix of the graph 
 *              in flattened form. The graph is updated in-place with the shortest path 
 *              distances. Infinity (`INF`) is used to indicate the absence of an edge.
 * @param vertices The number of vertices in the graph. The adjacency matrix is assumed 
 *                 to be of size `vertices x vertices`.
 * 
 * @details
 * The algorithm performs three nested loops:
 * - Outer loop: Iterates over each vertex `k`, treating it as an intermediate vertex.
 * - Middle loop: Iterates over each source vertex `i`.
 * - Inner loop: Iterates over each destination vertex `j`.
 * 
 * If the path from vertex `i` to vertex `j` through vertex `k` offers a shorter distance than 
 * the current path, the direct path is updated with the shorter distance. The addition is 
 * performed only when the paths through `k` are finite, ensuring correctness when dealing 
 * with disconnected nodes.
 * 
 * @note The input graph must be initialized with appropriate weights, where:
 * - `graph[i * vertices + j]` is the weight of the edge from vertex `i` to vertex `j`.
 * - `INF` represents no direct edge between the vertices.
 * 
 * @complexity
 * - Time complexity: O(V^3), where V is the number of vertices.
 * - Space complexity: O(V^2), as the adjacency matrix is stored in memory.
 * 
 * @usage
 * This function is suitable for small to medium-sized graphs or when parallelization 
 * is unnecessary or unavailable. For larger graphs, consider using a parallelized 
 * implementation (e.g., `naive_floyd_warshall`).
 */
void serial_floyd_warshall(
    std::vector<int> & graph,
    int vertices
);

#endif