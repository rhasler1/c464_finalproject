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

/**
 * @brief Computes the average of the timestamp values from a collection of labeled timestamps.
 * 
 * This function calculates the average of the numerical values (e.g., execution times) stored 
 * in the second element of each tuple in the provided vector of timestamps. Each tuple consists 
 * of a label (string) and a numerical value (double).
 * 
 * @param timestamps A reference to a vector of tuples, where each tuple contains:
 *                   - A label (string): Describes the timestamp (e.g., "Sequential time").
 *                   - A numerical value (double): The timestamp or execution time.
 * 
 * @return double The average of the numerical values in the timestamps. If the vector is empty, 
 *                the behavior of this function is undefined.
 * 
 * @details
 * - The function iterates over the vector using `std::for_each` and an inline lambda function 
 *   to accumulate the sum of the numerical values and count the total number of elements.
 * - The average is computed as `sum / count` and returned as a double.
 * 
 * @note
 * - The function assumes that the `timestamps` vector is non-empty. If the vector is empty, 
 *   the division by zero will result in undefined behavior.
 * - The input vector must contain valid tuples with numerical values as the second element.
 */
double compute_average(
    std::vector<std::tuple<std::string, double>> & timestamps
);

#endif