#ifndef __DATA_H__
#define __DATA_H__

#include <algorithm>
#include <complex>
#include <fstream>
#include <iomanip>
#include <iostream>

// Read data from a file.
template <typename T>
void read(const char *file_name, T *&data, unsigned &width, unsigned &height) {
    // Open the file.
    std::fstream fs;
    fs.open(file_name, std::fstream::in);
    if(!fs.is_open()) {
        std::cerr << "Error: failed to open " << file_name << std::endl;
        exit(1);
    }
    // Read the dimension information.
    fs >> width >> height;
    // Reserve vector space.
    data = new T[width * height]();
    // Read input data.
    for_each(data, data + (width * height), [&fs](T &d) { fs >> d; });
    // Close the file.
    fs.close();
}

// Write data into a file.
template <typename T>
void write(const char *file_name, T *data, const unsigned width, const unsigned height) {
    // Open the file.
    std::fstream fs;
    fs.open(file_name, std::fstream::out);
    if(!fs.is_open()) {
        std::cerr << "Error: failed to open " << file_name << std::endl;
        exit(1);
    }
    // Write the dimension information.
    fs << width << " " << height << std::endl;
    // Write data.
    for(unsigned r = 0; r < height; r++) {
        for(unsigned c = 0; c < width; c++) {
            fs << std::setprecision(1) << std::fixed
               << std::abs(data[r * width + c]) << " ";
        }   fs << std::endl;
    }
    // Close the file.
    fs.close();
}

// Deallocate the data array.
template <typename T>
void fin(T *data) { delete [] data; }

#endif

