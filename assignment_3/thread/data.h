#ifndef __DATA_H__
#define __DATA_H__

#include <algorithm>
#include <fstream>
#include <iostream>

template<typename T1, typename T2>
void load(const char *file_name, T1 *&array, T2 &size) {
    std::fstream fs;                            // Open a file.
    fs.open(file_name, std::fstream::in|std::fstream::binary);
    if(!fs.is_open()) {
        std::cerr << "Error: failed to open " << file_name << std::endl; std::exit(1);
    }
    fs.read((char*)&size, sizeof(T2));          // Read the number of data points.
    array = new T1[size];                       // Allocate the array.
    fs.read((char*)array, sizeof(T1)*size);     // Load data points.
    fs.close();                                 // Close the file.
}

template <typename T1, typename T2>
void fin(T1 *array, const T2 size) {
    if(!std::is_sorted(array, array + size)) {  // Validate if the array is sorted.
        std::cerr << "Error: array is not sorted" << std::endl; std::exit(1);
    }
    std::cout << "Done: array is sorted!" << std::endl;
    delete [] array;                            // Deallocate the array.
}

#endif

