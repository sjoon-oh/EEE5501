#include <cstdint>
#include <iostream>
#include "data.h"
#include "sort.h"
#include "stopwatch.h"

int main(int argc, char **argv) {
    if((argc < 2) || (argc > 3)) {                          // Run command message
        std::cerr << "Usage: " << argv[0] << " <num_threads> <data_file>" << std::endl;
        exit(1);
    }

    unsigned num_threads  = (unsigned)std::stoi(argv[1]);   // Number of threads
    const char *data_file = argc == 3 ? argv[2] : "data";   // Data file
    int *array = 0; uint64_t size = 0;                      // Data array and size
    unsigned n = num_threads;                               // Max num_threads is 1024.
    if(n > 1024) {
        std::cerr << "Error: too many threads" << std::endl;
        exit(1);
    }
    while(!(n & (unsigned)1)) { n = n >> 1; }               // num_threads must be power of two.
    if(n != 1) {
        std::cerr << "Error: num_threads is not power of two" << std::endl;
        exit(1);
    }

    load(data_file, array, size);                           // Load data to the array.

    stopwatch_t stopwatch;                                  // Measure time spent on sorting.
    stopwatch.start();
    sort(array, size, num_threads);                         // Sort the array.
    stopwatch.stop();
    stopwatch.display();

    fin(array, size);                                       // Finalize.

    return 0;
}

