#ifndef KERNEL_H
#define KERNEL_H

#include <vector>

int block_idx(
    int i,
    int j,
    int b
);

void floyd(
    std::vector<int> &C,
    const std::vector<int> &A,
    const std::vector<int> &B,
    int b
);

void blocked_floyd_warshall(
    std::vector<int> &W,
    int n,
    int b
);

void naive_floyd_warshall(
    std::vector<int> &graph,
    int vertices
);


void serial_floyd_warshall(
    std::vector<int> &graph,
    int vertices
);

#endif