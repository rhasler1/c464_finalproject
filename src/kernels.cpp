#include "kernels.h"
#include "globals.h"
#include <omp.h>

/**
 * @brief Computes the index for a flattened 2D block in a blocked matrix.
 * 
 * This helper function calculates the index of an element in a flattened
 * representation of a 2D block matrix based on its row (`i`), column (`j`), 
 * and the block size (`b`).
 * 
 * @param i The row index of the element within the block.
 * @param j The column index of the element within the block.
 * @param b The size (dimensions) of the block.
 * @return int The computed index in the flattened block.
 * 
 * This function is primarily used for indexing within the `blocked_floyd_warshall` implementation.
 */
static int block_idx(int i, int j, int b) {
    return i * b + j;
}

/**
 * @brief Performs the Floyd-Warshall algorithm on a single block of a matrix.
 * 
 * This function computes the shortest paths within a single block of a graph's 
 * adjacency matrix, represented in a flattened format. It updates the block matrix `C` 
 * with the minimum distances using the input blocks `A` and `B`.
 * 
 * @param C A reference to a vector representing the output block (distance matrix) in flattened format.
 * @param A A reference to a vector representing the first input block (distance matrix) in flattened format.
 * @param B A reference to a vector representing the second input block (distance matrix) in flattened format.
 * @param b The size (dimensions) of the block.
 * 
 * The algorithm iterates over the elements of the blocks to compute the minimum distances 
 * based on the formula: `C[i][j] = min(C[i][j], A[i][k] + B[k][j])`. It assumes all matrices 
 * are flattened and indexed using `block_idx`.
 */
static void floyd(std::vector<int> &C, const std::vector<int> &A, const std::vector<int> &B, int b) {
    for (int k = 0; k < b; ++k) {
        for (int j = 0; j < b; ++j) {
            for (int i = 0; i < b; ++i) {
                int c_idx = block_idx(i, j, b);
                int a_idx = block_idx(i, k, b);
                int b_idx = block_idx(k, j, b);

                // Check if paths exist before performing adition
                if
                (
                    A[a_idx] != INF &&
                    B[b_idx] != INF
                )
                {
                    C[c_idx] = std::min(C[c_idx], A[a_idx] + B[b_idx]);
                }
            }
        }
    }
}

void blocked_floyd_warshall(std::vector<int> &W, int n, int b) {
    // Number of blocks along one dimension
    int B = n / b;

    // Iterate over all block rows and columns
    for (int k = 0; k < B; ++k) {
        // Dependent Phase: Process block W[k][k]
        std::vector<int> Wkk(b * b);
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
                std::vector<int> Wkj(b * b), Wkj_tmp(b * b);
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
                std::vector<int> Wik(b * b), Wik_tmp(b * b);
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
                        std::vector<int> Wij(b * b), Wik(b * b), Wkj(b * b);
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

void naive_floyd_warshall(std::vector<int> &graph, int vertices)
{
    for (int k = 0; k < vertices; k++) {
        #pragma omp parallel for schedule(static)
        for (int i = 0; i < vertices; i++) {
            for (int j = 0; j < vertices; j++) {
                if
                (
                    graph[i * vertices + j] > (graph[i * vertices + k] + graph[k * vertices + j]) &&
                    graph[k * vertices + j] != INF &&
                    graph[i * vertices + k] != INF
                )
                {
                    graph[i * vertices + j] = graph[i * vertices + k] + graph[k * vertices + j];
                }
            }
        }
    }
}

void serial_floyd_warshall(std::vector<int> &graph, int vertices)
{
    for (int k = 0; k < vertices; k++) {
        for (int i = 0; i < vertices; i++) {
            for (int j = 0; j < vertices; j++) {
                if
                (
                    graph[i * vertices + j] > (graph[i * vertices + k] + graph[k * vertices + j]) &&
                    graph[k * vertices + j] != INF &&
                    graph[i * vertices + k] != INF
                )
                {
                    graph[i * vertices + j] = graph[i * vertices + k] + graph[k * vertices + j];
                }
            }
        }
    }
}