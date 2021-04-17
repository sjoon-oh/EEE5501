#include <iostream>
#include "array.h"

#define Length 9

const char *msg[Length] = {"Yonsei", "University", "is", "located", "in",
                           "Seoul,", "Republic",   "of", "Korea."};

int main(int argc, char **argv) {
    // Create a class instance, a1.
    array_t a1;

    // Push back input strings into a1.
    for(int i = 0; i < Length; i++) { a1.push_back(msg[i]); }

    // Create a class instance, a2, as a copy of a1.
    array_t a2 = a1;

    // Replace the zeroth and third strings of a2 with "Korea" and "also".
    if(a2.size()) { a2[0] = "Korea"; a2[3] = "also"; }

    // Print the string message stored in a1 and a2.
    for(size_t i = 0; i < a1.size(); i++) { std::cout << a1[i] << " "; }
    std::cout << std::endl;
    for(size_t i = 0; i < a2.size(); i++) { std::cout << a2[i] << " "; }
    std::cout << std::endl;

    return 0;
}

