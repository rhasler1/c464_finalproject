#include "timestamps.h"
#include <algorithm>
#include <chrono>
#include <fmt/format.h>

void mark_time(std::vector<std::tuple<std::string, double>> & timestamps, double & timestamp, const std::string_view label)
{
    timestamps.push_back(
        std::pair(
            label.data(),
            timestamp
        )
    );
}

void print_timestamps(std::vector<std::tuple<std::string, double>> & timestamps)
{
    std::for_each(timestamps.begin(), timestamps.end(), [](const auto & tuple) {
        fmt::print(
            "{}: {} ns",
            std::get<0>(tuple),
            std::get<1>(tuple)
        );
        fmt::print("\n");
    });
}

double compute_average(std::vector<std::tuple<std::string, double>> & timestamps)
{
    double avg = 0.0;
    double sum = 0.0;
    int count = 0;
    std::for_each(timestamps.begin(), timestamps.end(), [&](const auto & tuple) {
        sum += std::get<1>(tuple);
        count++;
    });
    return (double)(sum/count);
}