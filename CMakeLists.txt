# Minimum version of CMake
cmake_minimum_required(VERSION 3.28)

# Project name and language
project(
	HPCFinalProjectF24
	VERSION 0.1
	DESCRIPTION "Floyd-Warshall: A cache optimization analysis"
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
# FMT for formatting output
FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG        8.1.1
)
FetchContent_MakeAvailable(fmt)

# SPDLOG for logging
# Lots of compilation errors without this setting
option(SPDLOG_FMT_EXTERNAL "Use external fmt library instead of bundled" ON)
FetchContent_Declare(
        spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog.git
        GIT_TAG        v1.9.2
)
FetchContent_MakeAvailable(spdlog)

# CLI for parsing user input
FetchContent_Declare(
        cli11
        GIT_REPOSITORY https://github.com/CLIUtils/CLI11.git
        GIT_TAG        v2.1.2
)
FetchContent_MakeAvailable(cli11)

# GOOGLETEST for unit testing
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)

# Add subdirectories here:
add_subdirectory(src)

