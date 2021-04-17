#ifndef __VECTOR_HPP__
#define __VECTOR_HPP__

#include <cstdlib>
#include <new>

// Constructor
template <typename T>
vector_t<T>::vector_t(void) :
    array(0),
    array_size(0),
    num_elements(0) {
    // Nothing to do
}

// Copy constructor
template <typename T>
vector_t<T>::vector_t(const vector_t<T> &m_vector) :
    array(0),
    array_size(m_vector.num_elements),
    num_elements(m_vector.num_elements) {
    // Copy constructor creates a copy of tight-fit array.
    array = (T*)malloc(sizeof(T) * array_size);
    for(size_t i = 0; i < num_elements; i++) {
        new (&array[i]) T(m_vector.array[i]);
    }
}

// Destructor
template <typename T>
vector_t<T>::~vector_t(void) {
    // Destruct all elements first, and then free the array.
    for(size_t i = 0; i < num_elements; i++) { array[i].~T(); }
    free(array);
}

// Get the number of elements in the array.
template <typename T>
inline size_t vector_t<T>::size(void) const { return num_elements; }

// Get the allocated size of array in unit of elements.
template <typename T>
inline size_t vector_t<T>::capacity(void) const { return array_size; }

// Get a reference of element at the given index.
template <typename T>
inline T& vector_t<T>::operator[](const size_t m_index) const { return array[m_index]; }

// Get an iterator pointing to the first element of array.
template <typename T>
inline typename vector_t<T>::iterator vector_t<T>::begin(void) const {
    return iterator_t<T>(array);
}

// Get an iterator pointing to the next of last element.
template <typename T>
inline typename vector_t<T>::iterator vector_t<T>::end(void) const {
    return iterator_t<T>(array+num_elements);
}


/*************************
 * EEE5501: Assignment 2 *
 *************************/

// Author: SukJoon Oh, 2018142216
// Contact me through, sjoon.oh@pm.me
//
// README:
// ctor, copy ctor, dtor,
// size(), capacity(), begin(), end(), operator[] are provided.
// Plz only touch those commented with /* Assignment */
//

// Reserve an array space for the given number of elements.
template <typename T>
void vector_t<T>::reserve(size_t m_array_size) {
    /* Assignment */

    T* ic = 0;
	if (m_array_size > array_size) {				
		ic = (T*)malloc(m_array_size * sizeof(T)); // Newbie

		for (unsigned i = 0; i < num_elements; i++) {
			ic[i] = this->array[i]; // copies.
			array[i].~T(); // Same explicit call for same reason stated above.
		}

		free(this->array); // free
		this->array = ic; // Update the handle

        // Update the size info.
		this->array_size = m_array_size;
	}
}

// Remove all elements in the array.
template <typename T>
void vector_t<T>::clear(void) {
    /* Assignment */

    // Elements seated in the memory should be deleted.
    // In this case, they are the objects created from the constructors
    // thus explicit calling of dtors must be done.
    for (unsigned i = 0; i < num_elements; i++) 
        this->array[i].~T();
    
    num_elements = 0;
}

// Add a new element at the end of array.
template <typename T>
void vector_t<T>::push_back(const T &m_data) {
    /* Assignment */
    // First check whether free seat is available or not.
    if (this->num_elements < this->array_size) ; // Pass
    else if (this->array_size == 0) reserve(1);
    else reserve(2 * this->array_size);
        // No need to update array_size, since it is done inside of reserve().
    
    array[num_elements++] = m_data; // have a seat! 
}

// Remove the last element in the array.
template <typename T>
void vector_t<T>::pop_back(void) {
    /* Assignment */
    if ((num_elements) == 0) return; // Case when there is no element to delete.
    this->array[num_elements-- - 1].~T(); // Update the number of element.
}

// Assign new contents to the array.
template <typename T>
vector_t<T>& vector_t<T>::operator=(const vector_t<T> &m_vector) {
    /* Assignment */

    if (this == &m_vector) return *this; // Check the address and return myself~
    if (array_size) { // Case when the array is not empty. 
        // only need to look at the array_size, since array_size will hold heap address
        // if the value is non-zero.
        for (unsigned i = 0; i < num_elements; i++) array[i].~T(); // Same explicit call for same reason stated above.
		free(this->array); // free and exchange the handle
    }

    // Otherwise, Update infos.
    this->num_elements = m_vector.num_elements;

    this->array = (T*)malloc(sizeof(T) * m_vector.array_size); // Alloc
    for(size_t i = 0; i < num_elements; i++) new (&array[i]) T(m_vector.array[i]); // Copy them

    return *this;
}

// Add a new element at the location pointed by the iterator.
template <typename T>
typename vector_t<T>::iterator 
vector_t<T>::insert(vector_t<T>::iterator m_it, 
                    const T &m_data) {
    /* Assignment */

    // Think about the case when insert is called.
    // In any case, the size of an array element always gets increased to +1.

    // Two case exists.
    // The first case when there is already plenty seats.
    // The second case when the seats are already full.
    // Both cases are handled inside of reserve() function.
    
    if (array_size == num_elements) array_size++;
    
    this->reserve(array_size);
    new (&array[num_elements]) T(); // Make a dummy. 
    
    T* it = &array[num_elements]; // First point to the very rear. Same as end().
    
    if (num_elements++) while( &(*m_it) != it-- ) { *(it + 1) = *it; }
    *m_it = m_data; // Move forward

    return m_it;
}

// Erase an element at the location pointed by the iterator.
template <typename T>
typename vector_t<T>::iterator 
vector_t<T>::erase(vector_t<T>::iterator m_it) {
    /* Assignment */    
    T* it = &(*m_it); // First point to the target address.

    // Move to next
    // Search and move element forward.
    while( &array[num_elements - 1] > it++ ) *(it - 1) = *(it);
    array[num_elements-- - 1].~T();
    
    return m_it;
}

/*********************
 * End of Assignment *
 *********************/

#endif

