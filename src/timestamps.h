#ifndef TIMESTAMPS_H
#define TIMESTAMPS_H

#include <vector>
#include <string>
#include <unordered_map>

// mark_time using nanotimer
void mark_time(std::vector<std::tuple<std::string, double>> & timestamps, double & timestamp, std::string_view label);

// print_timestamps using nanotimer
void print_timestamps(std::vector<std::tuple<std::string, double>> & timestamps);

#endif