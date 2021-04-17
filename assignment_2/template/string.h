#include <cstring>
#include <iostream>

// Minimal implementation of string
class string_t {
public:
    string_t();
    string_t(const string_t &m_string);
    string_t(const char *m_char);
    ~string_t();

    string_t& operator=(const string_t &m_string);
    string_t& operator=(const char *m_char);
    bool operator<(const string_t &m_string) const;
    bool operator>(const string_t &m_string) const;
    char& operator[](const size_t m_index) const;

private:
    char *ptr;
    size_t length;

friend std::ostream& operator<<(std::ostream &m_os, const string_t &m_string);
};

inline bool string_t::operator<(const string_t &m_string) const {
    return strcmp(ptr, m_string.ptr) < 0;
}

inline bool string_t::operator>(const string_t &m_string) const {
    return strcmp(ptr, m_string.ptr) > 0;
}

inline char& string_t::operator[](const size_t m_index) const {
    return ptr[m_index];
}

inline std::ostream& operator<<(std::ostream &m_os, const string_t &m_string) {
    return m_os << m_string.ptr;
}

