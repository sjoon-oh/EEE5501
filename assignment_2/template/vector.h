#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "iterator.h"

template <typename T>
class vector_t {
public:
    // Constructor
    vector_t(void);
    // Copy constructor
    vector_t(const vector_t<T> &m_vector);
    // Destructor
    ~vector_t(void);

    // vector_t<T>::iterator
    typedef iterator_t<T> iterator;

    // Get the number of elements in the array.
    size_t size(void) const;
    // Get the allocated size of array in unit of elements.
    size_t capacity(void) const;
    // Reserve an array space for the given number of elements.
    void reserve(size_t m_array_size);
    // Remove all elements in the array.
    void clear(void);
    // Add a new element at the end of array.
    void push_back(const T &m_data);
    // Remove the last element in the array.
    void pop_back(void);
    // Assign new contents to the array.
    vector_t<T>& operator=(const vector_t<T> &m_vector);
    // Get a reference of element at the given index.
    T& operator[](const size_t m_index) const;
    // Get an iterator pointing to the first element of array.
    iterator begin(void) const;
    // Get an iterator pointing to the next of last element.
    iterator end(void) const;
    // Add a new element at the location pointed by the iterator.
    iterator insert(iterator m_it, const T &m_data);
    // Erase an element at the location pointed by the iterator.
    iterator erase(iterator m_it);

private:
    T *array;               // Data array
    size_t array_size;      // Allocated array size
    size_t num_elements;    // Number of elements in the array
};

#include "vector.hpp"

#endif

