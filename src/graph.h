#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

void graph_init(
    std::vector<int> & graph,
    int vertices
);

int generate_linear_graph(
    std::vector<int> & graph,
    int vertices,
    int edges
);

#endif