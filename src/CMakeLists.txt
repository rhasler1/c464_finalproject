# Add executable
add_executable(
    floyd_warshall
    main.cpp
    graph.cpp
    timestamps.cpp
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
# enable_testing() # uncomment after testing has been implemented
# TODO
