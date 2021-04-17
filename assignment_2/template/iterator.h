#ifndef __VECTOR_ITERATOR_H__
#define __VECTOR_ITERATOR_H__

template <typename T> class vector_t;

template <typename T>
class iterator_t {
friend class vector_t<T>; 
public:
    iterator_t(void) : ptr(0) { /* Nothing to do */ }
    iterator_t(T *m_ptr) : ptr(m_ptr) { /* Nothing to do */ }
    iterator_t(const iterator_t<T> &m_it) : ptr(m_it.ptr) { /* Nothing to do */ }
    ~iterator_t(void) { /* Nothing to do */ }

    // Deference operator
    T& operator*(void) const { return *ptr; }
    // Prefix increment operator
    iterator_t<T> operator++(void) { ++ptr; return *this; }
    // Postfix increment operator
    iterator_t<T> operator++(int) { return iterator_t<T>(ptr++); }
    // Prefix decrement operator
    iterator_t<T> operator--(void) { --ptr; return *this; }
    // Postfix decrement operator
    iterator_t<T> operator--(int) { return iterator_t<T>(ptr--); }
    // Equal operator
    bool operator==(const iterator_t<T> &m_it) const { return ptr == m_it.ptr; }
    // Not-equal operator
    bool operator!=(const iterator_t<T> &m_it) const { return ptr != m_it.ptr; }

private:
    T *ptr;
};

#endif

