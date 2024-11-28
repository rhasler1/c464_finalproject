#ifndef TIMESTAMPS_H
#define TIMESTAMPS_H

#include <vector>
#include <string>

/**
 * @brief Records a labeled timestamp into a collection of timestamps.
 * 
 * @param timestamps A reference to a vector of tuples where each tuple contains a label (string) and a timestamp (double).
 * @param timestamp A reference to the current timestamp value (double) to be recorded.
 * @param label A string view representing the label for the timestamp.
 * 
 * This function appends a labeled timestamp to the provided `timestamps` vector. Each entry consists of a label
 * (e.g., a description of the event) and a timestamp (e.g., the time the event occurred).
 * */
void mark_time(
    std::vector<std::tuple<std::string, double>> & timestamps,
    double & timestamp,
    std::string_view label
);

/**
 * @brief Outputs the labeled timestamps to the console.
 * 
 * @param timestamps A reference to a vector of tuples where each tuple contains a label (string) and a timestamp (double).
 * 
 * This function iterates over the `timestamps` vector and prints each labeled timestamp in the format "label,timestamp".
 * Each entry is printed on a new line. The function uses the `fmt::print` function for output.
 */
void print_timestamps(
    std::vector<std::tuple<std::string, double>> & timestamps
);

#endif