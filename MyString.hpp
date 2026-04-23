
#ifndef SIMPLE_STRING_SIMPLESTRING_HPP
#define SIMPLE_STRING_SIMPLESTRING_HPP

#include <stdexcept>
#include <cstring>
#include <algorithm>

class MyString {
private:
    union {
        char* heap_ptr;
        char small_buffer[16];
    };
    size_t _size;
    size_t _capacity; // Internal capacity. If <= 15, we are in SSO mode.

    bool is_small() const {
        return _capacity <= 15;
    }

    char* get_data() {
        return is_small() ? small_buffer : heap_ptr;
    }

    const char* get_data() const {
        return is_small() ? small_buffer : heap_ptr;
    }

public:
    MyString() : _size(0), _capacity(15) {
        small_buffer[0] = '\0';
    }

    MyString(const char* s) {
        _size = strlen(s);
        if (_size <= 15) {
            _capacity = 15;
            memcpy(small_buffer, s, _size + 1);
        } else {
            _capacity = _size;
            heap_ptr = new char[_capacity + 1];
            memcpy(heap_ptr, s, _size + 1);
        }
    }

    MyString(const MyString& other) : _size(other._size), _capacity(other._capacity) {
        if (other.is_small()) {
            memcpy(small_buffer, other.small_buffer, _size + 1);
        } else {
            heap_ptr = new char[_capacity + 1];
            memcpy(heap_ptr, other.heap_ptr, _size + 1);
        }
    }

    MyString(MyString&& other) noexcept : _size(other._size), _capacity(other._capacity) {
        if (other.is_small()) {
            memcpy(small_buffer, other.small_buffer, _size + 1);
        } else {
            heap_ptr = other.heap_ptr;
            other.heap_ptr = nullptr;
        }
        other._size = 0;
        other._capacity = 15;
        other.small_buffer[0] = '\0';
    }

    MyString& operator=(MyString&& other) noexcept {
        if (this != &other) {
            if (!is_small()) {
                delete[] heap_ptr;
            }
            _size = other._size;
            _capacity = other._capacity;
            if (other.is_small()) {
                memcpy(small_buffer, other.small_buffer, _size + 1);
            } else {
                heap_ptr = other.heap_ptr;
                other.heap_ptr = nullptr;
            }
            other._size = 0;
            other._capacity = 15;
            other.small_buffer[0] = '\0';
        }
        return *this;
    }

    MyString& operator=(const MyString& other) {
        if (this != &other) {
            if (other.is_small()) {
                if (!is_small()) {
                    delete[] heap_ptr;
                }
                _size = other._size;
                _capacity = 15;
                memcpy(small_buffer, other.small_buffer, _size + 1);
            } else {
                char* new_ptr = new char[other._capacity + 1];
                memcpy(new_ptr, other.heap_ptr, other._size + 1);
                if (!is_small()) {
                    delete[] heap_ptr;
                }
                _size = other._size;
                _capacity = other._capacity;
                heap_ptr = new_ptr;
            }
        }
        return *this;
    }

    ~MyString() {
        if (!is_small()) {
            delete[] heap_ptr;
        }
    }

    const char* c_str() const {
        return get_data();
    }

    size_t size() const {
        return _size;
    }

    size_t capacity() const {
        if (_size <= 15) return 15;
        return _capacity;
    }

    void reserve(size_t new_capacity) {
        if (new_capacity <= _capacity) return;
        if (new_capacity <= 15) return;

        char* new_ptr = new char[new_capacity + 1];
        memcpy(new_ptr, get_data(), _size + 1);
        if (!is_small()) {
            delete[] heap_ptr;
        }
        heap_ptr = new_ptr;
        _capacity = new_capacity;
    }

    void resize(size_t new_size) {
        if (new_size <= _size) {
            _size = new_size;
            get_data()[_size] = '\0';
            if (new_size <= 15 && !is_small()) {
                char* old_ptr = heap_ptr;
                memcpy(small_buffer, old_ptr, _size + 1);
                delete[] old_ptr;
                _capacity = 15;
            }
        } else {
            if (new_size > _capacity) {
                reserve(std::max(new_size, _capacity * 2));
            }
            char* data = get_data();
            for (size_t i = _size; i < new_size; ++i) {
                data[i] = '\0';
            }
            _size = new_size;
            data[_size] = '\0';
        }
    }

    char& operator[](size_t index) {
        if (index >= _size) throw std::out_of_range("MyString::operator[]");
        return get_data()[index];
    }

    const char& operator[](size_t index) const {
        if (index >= _size) throw std::out_of_range("MyString::operator[]");
        return get_data()[index];
    }

    MyString operator+(const MyString& rhs) const {
        MyString res(*this);
        res.append(rhs.c_str());
        return res;
    }

    void append(const char* str) {
        size_t len = strlen(str);
        if (_size + len > _capacity) {
            reserve(std::max(_size + len, _capacity * 2));
        }
        memcpy(get_data() + _size, str, len + 1);
        _size += len;
    }

    const char& at(size_t pos) const {
        if (pos >= _size) throw std::out_of_range("MyString::at");
        return get_data()[pos];
    }

    char& at(size_t pos) {
        if (pos >= _size) throw std::out_of_range("MyString::at");
        return get_data()[pos];
    }

    class const_iterator;

    class iterator {
    private:
        char* ptr;
    public:
        friend class MyString;
        friend class const_iterator;
        iterator(char* p) : ptr(p) {}
        iterator& operator++() {
            ++ptr;
            return *this;
        }
        iterator operator++(int) {
            iterator tmp = *this;
            ++ptr;
            return tmp;
        }
        iterator& operator--() {
            --ptr;
            return *this;
        }
        iterator operator--(int) {
            iterator tmp = *this;
            --ptr;
            return tmp;
        }
        char& operator*() const {
            return *ptr;
        }
        char* operator->() const {
            return ptr;
        }
        bool operator==(const iterator& other) const {
            return ptr == other.ptr;
        }
        bool operator!=(const iterator& other) const {
            return ptr != other.ptr;
        }
        bool operator==(const const_iterator& other) const;
        bool operator!=(const const_iterator& other) const;
    };

    class const_iterator {
    private:
        const char* ptr;
    public:
        friend class MyString;
        friend class iterator;
        const_iterator(const char* p) : ptr(p) {}
        const_iterator(const iterator& it) : ptr(it.ptr) {}
        const_iterator& operator++() {
            ++ptr;
            return *this;
        }
        const_iterator operator++(int) {
            const_iterator tmp = *this;
            ++ptr;
            return tmp;
        }
        const_iterator& operator--() {
            --ptr;
            return *this;
        }
        const_iterator operator--(int) {
            const_iterator tmp = *this;
            --ptr;
            return tmp;
        }
        const char& operator*() const {
            return *ptr;
        }
        const char* operator->() const {
            return ptr;
        }
        bool operator==(const const_iterator& other) const {
            return ptr == other.ptr;
        }
        bool operator!=(const const_iterator& other) const {
            return ptr != other.ptr;
        }
        bool operator==(const iterator& other) const {
            return ptr == other.ptr;
        }
        bool operator!=(const iterator& other) const {
            return ptr != other.ptr;
        }
    };

    iterator begin() {
        return iterator(get_data());
    }

    iterator end() {
        return iterator(get_data() + _size);
    }

    const_iterator begin() const {
        return const_iterator(get_data());
    }

    const_iterator end() const {
        return const_iterator(get_data() + _size);
    }

    const_iterator cbegin() const {
        return const_iterator(get_data());
    }

    const_iterator cend() const {
        return const_iterator(get_data() + _size);
    }
};

inline bool MyString::iterator::operator==(const const_iterator& other) const {
    return ptr == other.ptr;
}
inline bool MyString::iterator::operator!=(const const_iterator& other) const {
    return ptr != other.ptr;
}

#endif
