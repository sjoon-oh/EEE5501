#include "array.h"
// #include <cstring>

// Class constructor
array_t::array_t() :
    ptr(0),
    num_elements(0),
    array_size(0) {
}


/* Assignment 1:
   Write a copy constructor and destructor of the class,
   and complete reserve() and push_back() functions.
  */
// copy ctor
array_t::array_t(array_t& ref) {
	
	if (ref.ptr == 0)	; // case when not allocated. Do nothing.
	else {
		
		// When some data exists, first allocate the memory to heap,
		// then copy the contents.
		ptr = (data_t*)malloc(ref.array_size * sizeof(data_t) );

		for (unsigned i = 0; i < ref.num_elements; i++)	
			ptr[i] = ref.ptr[i];
		
		// Note that the object created is user-defined. 
		// It cannot be deep-copied just by calling memcpy() nor strcpy() etc.
		// Since the string_t class has a copy constructor in itself, 
		// call the overriden function operator= to make an exact twin of itself.

		num_elements = ref.num_elements;
		array_size = ref.array_size;
	}
}

// dtor
array_t::~array_t() {

	if (this->ptr != 0) {
		for (unsigned i = 0; i < num_elements; i++) ptr[i].~string_t();
		
		// Same here, with the user-defined class.
		// Since malloc is just for making a room in memory, 
		// simple calling free() does not wake the destructor of each object.
		// Thus, the destructor of each object lied in memory should be called explicitly.

		free(this->ptr);
	}
}


// Allocate a memory space for the specified number of elements.
void array_t::reserve(const size_t m_array_size) {	
	data_t* intermediate_container;
	
	if (m_array_size > this->num_elements) {				

		intermediate_container = (data_t*)malloc(m_array_size * sizeof(data_t));

		for (unsigned i = 0; i < num_elements; i++) {	
			intermediate_container[i] = ptr[i];
			ptr[i].~string_t(); // Same explicit call for same reason stated above.
		}

		// free and exchange the handle
		free(this->ptr);
		this->ptr = intermediate_container;

		this->array_size = m_array_size;
	}
}

// Add a new element at the end of array.
void array_t::push_back(const data_t m_value) {

	if (!(num_elements < array_size)) {
		// Increase the capacity
		if (num_elements == 0) // case when empty as initialization
			reserve(1);
		else this->reserve(2 * array_size); // increase the capacity
	}
	
	ptr[num_elements] = m_value;
	num_elements++;	
	
}

