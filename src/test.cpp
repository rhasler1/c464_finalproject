#include <spdlog/spdlog.h>
#include <gtest/gtest.h>
#include "graph.h"
#include "kernels.h"
#include "globals.h"
#include <omp.h>
#include <vector>

class FloydWarshallTest : public testing::Test {
    public:
        void SetUp() override {
            spdlog::set_level(spdlog::level::off);
            spdlog::info("spdlog level set to {}", spdlog::get_level());
        }

        void TearDown() override {
            graph_1.clear();
            graph_2.clear();
            graph_3.clear();
        }
    
    protected:
        int vertices{1000};
        int edges{200};
        int tile_length{20};
        int threads{2};

        std::vector<int> graph_1;
        std::vector<int> graph_2;
        std::vector<int> graph_3;      
};

TEST_F(FloydWarshallTest, TestAll)
{
    // Initialize graphs.
    graph_1.resize(vertices * vertices, INF);
    graph_2.resize(vertices * vertices, INF);
    graph_3.resize(vertices * vertices, INF);
    // Populate graph 1 with generated data.
    generate_linear_graph(graph_1, vertices, edges);
    // Copy data from graph 1 into graphs 2 and 3.
    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            graph_2[i * vertices + j] = graph_1[i * vertices + j];
            graph_3[i * vertices + j] = graph_1[i * vertices + j];
        }
    }
    // Set threads
    omp_set_num_threads(threads);
    // Run serial on graph 1.
    serial_floyd_warshall(graph_1, vertices);
    // Run naive parallel on graph 2.
    naive_floyd_warshall(graph_2, vertices);
    // Run blocked parallel on graph 3.
    blocked_floyd_warshall(graph_3, vertices, tile_length);
    // Compare results.
    for (int i = 0; i < vertices; i++) {
        for (int j = 0; j < vertices; j++) {
            ASSERT_EQ(graph_1[i * vertices + j], graph_2[i * vertices + j]);
            ASSERT_EQ(graph_2[i * vertices + j], graph_3[i * vertices + j]);
        }
    }
}