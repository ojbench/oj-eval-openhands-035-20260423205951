

#include "MyString.hpp"
#include <iostream>
#include <cassert>
#include <vector>

void test_basic() {
    MyString s1;
    assert(s1.size() == 0);
    assert(s1.capacity() == 15);
    assert(strcmp(s1.c_str(), "") == 0);

    MyString s2("hello");
    assert(s2.size() == 5);
    assert(s2.capacity() == 15);
    assert(strcmp(s2.c_str(), "hello") == 0);

    MyString s3("this is a long string that exceeds SSO");
    assert(s3.size() > 15);
    assert(s3.capacity() >= s3.size());
    assert(strcmp(s3.c_str(), "this is a long string that exceeds SSO") == 0);
}

void test_copy_move() {
    MyString s1("hello");
    MyString s2 = s1;
    assert(strcmp(s2.c_str(), "hello") == 0);
    assert(s2.size() == 5);

    MyString s3 = std::move(s1);
    assert(strcmp(s3.c_str(), "hello") == 0);
    assert(s1.size() == 0);

    MyString s4("long string long string long string");
    MyString s5 = s4;
    assert(strcmp(s5.c_str(), s4.c_str()) == 0);
    assert(s5.size() == s4.size());

    MyString s6 = std::move(s4);
    assert(s6.size() > 15);
    assert(s4.size() == 0);
}

void test_append_concat() {
    MyString s1("hello");
    s1.append(" world");
    assert(strcmp(s1.c_str(), "hello world") == 0);

    MyString s2 = s1 + MyString("!");
    assert(strcmp(s2.c_str(), "hello world!") == 0);
}

void test_iterator() {
    MyString s("abc");
    std::string res;
    for (auto it = s.begin(); it != s.end(); ++it) {
        res += *it;
    }
    assert(res == "abc");

    const MyString cs("def");
    res = "";
    for (auto it = cs.begin(); it != cs.end(); ++it) {
        res += *it;
    }
    assert(res == "def");
}

void test_resize_reserve() {
    MyString s("hello");
    s.resize(3);
    assert(strcmp(s.c_str(), "hel") == 0);
    assert(s.size() == 3);

    s.resize(10);
    assert(s.size() == 10);
    assert(s.c_str()[3] == '\0');

    s.reserve(100);
    // According to requirement: if size <= 15, capacity() should return 15
    assert(s.size() == 10);
    assert(s.capacity() == 15);

    // Test transition from heap to SSO
    MyString s2("this is a very long string that is definitely on the heap");
    assert(s2.size() > 15);
    s2.resize(5);
    assert(s2.size() == 5);
    assert(s2.capacity() == 15);
    assert(strcmp(s2.c_str(), "this ") == 0);
}

int main() {
    test_basic();
    test_copy_move();
    test_append_concat();
    test_iterator();
    test_resize_reserve();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}

