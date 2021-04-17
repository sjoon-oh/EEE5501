#include <iostream>
#include <fstream>
#include "string.h"
#include "vector.h"

#define WORD_SIZE 16

using namespace std;

// Print the information of vector_t<string_t>.
void print(const vector_t<string_t> &m_vector) {
    cout << "capacity()="   << m_vector.capacity()  << endl
         << "size()="       << m_vector.size()      << endl
         << "array=[ ";
    for(vector_t<string_t>::iterator it = m_vector.begin();
        it != m_vector.end(); it++)  { cout << *it  << " "; }
    cout << "]"                             << endl << endl;
}

int main(int argc, char **argv) {
    // Execution command message.
    if(argc > 2) { cerr << "Usage: " << argv[0] << " <input_file>" << endl; exit(1); }

    // Open a text file.
    ifstream input_file;
    input_file.open(argc == 2 ? argv[1] : "input");
    if(!input_file.is_open()) { cerr << "Error: failed to open " << argv[1] << endl; exit(1); }

    // Create string vectors.
    vector_t<string_t> words, sorted;

    // Push words to a vector, 'words'.
    char line[WORD_SIZE];
    while(input_file.getline(line, WORD_SIZE)) { words.push_back(line); }
    print(words);

    // Close the text file.
    input_file.close();

    // Remove words starting with 'a', 'e', 'i', 'o', 'u'.
    for(vector_t<string_t>::iterator it = words.begin(); it != words.end(); it++) {
        if(((*it)[0] == 'a') || ((*it)[0] == 'e') || ((*it)[0] == 'i') ||
           ((*it)[0] == 'o') || ((*it)[0] == 'u')) { words.erase(it--); }
    }
    print(words);

    // Insert words to a vector, 'sorted', in ascending order.
    sorted.reserve(words.size());
    for(size_t i = 0; i < words.size(); i++) {
        vector_t<string_t>::iterator it = sorted.begin();
        while((it != sorted.end()) && (words[i] > *it)) { it++; }
        sorted.insert(it, words[i]);
    }
    print(sorted);

    // Remove words starting with 'z'.
    while(sorted.size() && (sorted[sorted.size()-1][0] == 'z')) { sorted.pop_back(); }
    print(sorted);

    // Blow up everything.
    sorted.clear();
    print(sorted);

    // Copy the empty vector.
    words = sorted;
    print(words);
    
    return 0;
}

