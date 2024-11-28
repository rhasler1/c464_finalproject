#include "kernels.h"
#include <omp.h>

// Helper function to calculate 1D index in a block
int block_idx(int i, int j, int b) {
    return i * b + j;
}

// Floyd-Warshall on a single block (b × b matrix stored as a 1D vector)
void floyd(std::vector<int> &C, const std::vector<int> &A, const std::vector<int> &B, int b) {
    for (int k = 0; k < b; ++k) {
        for (int j = 0; j < b; ++j) {
            for (int i = 0; i < b; ++i) {
                int c_idx = block_idx(i, j, b);
                int a_idx = block_idx(i, k, b);
                int b_idx = block_idx(k, j, b);
                C[c_idx] = std::min(C[c_idx], A[a_idx] + B[b_idx]);
            }
        }
    }
}

// Blocked Floyd-Warshall
void blocked_floyd_warshall(std::vector<int> &W, int n, int b) {
    int B = n / b; // Number of blocks along one dimension

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

void serial_floyd_warshall(std::vector<int> &graph, int vertices)
{
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
}