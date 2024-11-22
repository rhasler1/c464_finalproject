# Add executable
add_executable(floyd_warshall main.cpp)

# Link libraries to executable
target_link_libraries(floyd_warshall fmt::fmt spdlog::spdlog CLI11::CLI11)

# Enable testing
# enable_testing() # uncomment after testing has been implemented
# TODO
