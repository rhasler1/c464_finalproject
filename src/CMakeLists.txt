# Add executable
add_executable(
    floyd_warshall
    main.cpp
    graph.cpp
    timestamps.cpp
    kernels.cpp
    globals.cpp
)

# Link libraries to executable
target_link_libraries(
    floyd_warshall
    fmt::fmt
    spdlog::spdlog
    CLI11::CLI11
    OpenMP::OpenMP_CXX
    ${OPENMP_LIBS}
)

# Add compile options for OpenMP
target_compile_options(floyd_warshall PRIVATE ${OPENMP_FLAGS})

# Enable testing
enable_testing() # uncomment after testing has been implemented
add_executable(tests test.cpp graph.cpp kernels.cpp globals.cpp)

target_link_libraries(
    tests
    gtest_main
    fmt::fmt
    spdlog::spdlog
    CLI11::CLI11
    OpenMP::OpenMP_CXX
    ${OPENMP_LIBS}
)

target_compile_options(tests PRIVATE ${OPENMP_FLAGS})

include(GoogleTest)
gtest_discover_tests(tests)

