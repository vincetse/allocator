#include "lazy/memory/buffer_allocator.h"
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE BufferAllocatorTest
#include <cstring>
#include <boost/test/unit_test.hpp>
#include <vector>
#include <string>
#include <list>
#include <ext/slist>
#include <deque>
#include <queue>
#include <map>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <utility>
#include <functional>
#include <algorithm>

BOOST_AUTO_TEST_CASE( stl_vector_push_back )
{
    typedef int data_type;
    typedef lazy::memory::buffer_allocator<data_type> allocator_type;

    const size_t num_objects = 3;
    data_type buffer[num_objects];
    allocator_type allocator(buffer, sizeof(buffer));
    BOOST_REQUIRE_EQUAL(allocator.max_size(), num_objects);

    std::vector<data_type, allocator_type> vec(allocator);
    BOOST_REQUIRE_EQUAL(vec.size(), 0);
    BOOST_REQUIRE_NO_THROW(vec.push_back(1));
    BOOST_REQUIRE_EQUAL(vec.size(), 1);
    BOOST_REQUIRE_NO_THROW(vec.push_back(2));
    BOOST_REQUIRE_EQUAL(vec.size(), 2);
    BOOST_REQUIRE_EQUAL(vec[0], 1);
    BOOST_REQUIRE_EQUAL(vec[1], 2);
    BOOST_REQUIRE_THROW(vec.push_back(3), std::bad_alloc);
    BOOST_REQUIRE_EQUAL(vec.size(), 2);
}

BOOST_AUTO_TEST_CASE( stl_vector_resize )
{
    typedef int data_type;
    typedef lazy::memory::buffer_allocator<data_type> allocator_type;

    const size_t num_objects = 2;
    data_type buffer[num_objects];
    allocator_type allocator(buffer, sizeof(buffer));
    BOOST_REQUIRE_EQUAL(allocator.max_size(), num_objects);

    std::vector<data_type, allocator_type> vec(allocator);
    BOOST_REQUIRE_EQUAL(vec.size(), 0);
    BOOST_REQUIRE_NO_THROW(vec.resize(num_objects));
    BOOST_REQUIRE_EQUAL(vec.size(), num_objects);
    BOOST_REQUIRE_THROW(vec.resize(num_objects + 1), std::length_error);
    BOOST_REQUIRE_EQUAL(vec.size(), num_objects);
}

BOOST_AUTO_TEST_CASE( stl_vector_copy )
{
    typedef int data_type;
    typedef lazy::memory::buffer_allocator<data_type> allocator_type;

    const size_t num_objects = 2;
    data_type buffer1[num_objects];
    allocator_type allocator1(buffer1, sizeof(buffer1));
    data_type buffer2[num_objects];
    allocator_type allocator2(buffer2, sizeof(buffer2));

    std::vector<data_type, allocator_type> vec1(allocator1);
    std::vector<data_type, allocator_type> vec2(allocator2);

    BOOST_REQUIRE_NO_THROW(vec1.reserve(num_objects));
    BOOST_REQUIRE_NO_THROW(vec1.push_back(1));
    BOOST_REQUIRE_NO_THROW(vec1.push_back(2));
    BOOST_REQUIRE_EQUAL(vec1[0], 1);
    BOOST_REQUIRE_EQUAL(vec1[1], 2);
    BOOST_REQUIRE_NO_THROW(vec2 = vec1);
    BOOST_REQUIRE_EQUAL(vec2[0], 1);
    BOOST_REQUIRE_EQUAL(vec2[1], 2);
    vec1.clear();
    memset(buffer1, 0, sizeof(buffer1));
    BOOST_REQUIRE_EQUAL(vec1.size(), 0);
    BOOST_REQUIRE_EQUAL(vec2[0], 1);
    BOOST_REQUIRE_EQUAL(vec2[1], 2);
}

BOOST_AUTO_TEST_CASE( stl_string )
{
    typedef char data_type;
    typedef lazy::memory::buffer_allocator<data_type> allocator_type;
    typedef std::basic_string<data_type, std::char_traits<data_type>, allocator_type> string_type;

    // DISCLAIMER:
    // I have not studied the behavior of how std::basic_string allocates memory when we reserve on
    // it or as it grows dynamically, so I am just allocating a "big enough" buffer and then use
    // a significantly smaller amount and hope that it is enough.
    const size_t buffer_size = 32 * 1024;
    data_type buffer[buffer_size];
    allocator_type allocator(buffer, sizeof(buffer));
    string_type str(allocator);
    BOOST_REQUIRE_EQUAL(str.length(), 0);
    BOOST_REQUIRE_NO_THROW(str.reserve(512));
    BOOST_REQUIRE_NO_THROW(str.append("hello,"));
    BOOST_REQUIRE_NO_THROW(str.append(" world!"));
    BOOST_REQUIRE_EQUAL(str, "hello, world!");
}

BOOST_AUTO_TEST_CASE( stl_string_copy )
{
    typedef char data_type;
    typedef lazy::memory::buffer_allocator<data_type> allocator_type;
    typedef std::basic_string<data_type, std::char_traits<data_type>, allocator_type> string_type;

    const size_t buffer_size = 32 * 1024;
    data_type buffer1[buffer_size];
    data_type buffer2[buffer_size];
    allocator_type allocator1(buffer1, sizeof(buffer1));
    allocator_type allocator2(buffer2, sizeof(buffer2));
    string_type str1(allocator1);
    string_type str2(allocator2);

    BOOST_REQUIRE_NO_THROW(str1 = "Hello, world!");
    BOOST_REQUIRE_EQUAL(str1, "Hello, world!");
    BOOST_REQUIRE_NO_THROW(str2 = str1);
    BOOST_REQUIRE_EQUAL(str1, str2);
    BOOST_REQUIRE_EQUAL(str1, "Hello, world!");
    BOOST_REQUIRE_EQUAL(str2, "Hello, world!");
    // basic_strings are reference-counted, so even after str1 is cleared, str2
    // will still hold the value but pointing to str1's buffer1.  If we messed
    // with str1's underlying buffer1, str2 will see the value and gets screwed.
    str1.clear();
    memset(buffer1, 0, sizeof(buffer1));
    BOOST_REQUIRE(str1.empty());
    BOOST_REQUIRE_EQUAL(str2.length(), 0);
    BOOST_REQUIRE_NE(str2, "Hello, world!");

    // basic_string copies on write, if we changed str2, it would stop using
    // str1's buffer1 and uses its own buffer2.
    BOOST_REQUIRE_NO_THROW(str1 = "The quick brown fox jumps over the lazy dog");
    BOOST_REQUIRE_EQUAL(str1, "The quick brown fox jumps over the lazy dog");
    BOOST_REQUIRE_NO_THROW(str2 = str1);
    BOOST_REQUIRE_EQUAL(str1, str2);
    BOOST_REQUIRE_EQUAL(str1, "The quick brown fox jumps over the lazy dog");
    BOOST_REQUIRE_EQUAL(str2, "The quick brown fox jumps over the lazy dog");
    BOOST_REQUIRE_NO_THROW(str2.append("!!!"));
    BOOST_REQUIRE_EQUAL(str1, "The quick brown fox jumps over the lazy dog");
    BOOST_REQUIRE_EQUAL(str2, "The quick brown fox jumps over the lazy dog!!!");
    str1.clear();
    memset(buffer1, 0, sizeof(buffer1));
    BOOST_REQUIRE(str1.empty());
    BOOST_REQUIRE_EQUAL(str2, "The quick brown fox jumps over the lazy dog!!!");
}

BOOST_AUTO_TEST_CASE( stl_list )
{
    typedef int data_type;
    typedef lazy::memory::buffer_allocator<data_type> allocator_type;
    typedef std::list<data_type, allocator_type> list_type;

    const size_t buffer_size = 32 * 1024;
    data_type buffer[buffer_size];
    allocator_type allocator(buffer, sizeof(buffer));
    list_type l(allocator);
    BOOST_REQUIRE_NO_THROW(l.push_back(1));
    BOOST_REQUIRE_NO_THROW(l.push_front(2));
    BOOST_REQUIRE_EQUAL(l.front(), 2);
    BOOST_REQUIRE_EQUAL(l.back(), 1);
    BOOST_REQUIRE_THROW(l.resize(buffer_size, 0), std::bad_alloc);
}

BOOST_AUTO_TEST_CASE( stl_slist )
{
    typedef int data_type;
    typedef lazy::memory::buffer_allocator<data_type> allocator_type;
    typedef __gnu_cxx::slist<data_type, allocator_type> list_type;

    const size_t buffer_size = 32 * 1024;
    data_type buffer[buffer_size];
    allocator_type allocator(buffer, sizeof(buffer));
    list_type l(allocator);
    BOOST_REQUIRE_NO_THROW(l.push_front(1));
    BOOST_REQUIRE_NO_THROW(l.push_front(2));
    BOOST_REQUIRE_EQUAL(l.front(), 2);
    BOOST_REQUIRE_THROW(l.resize(buffer_size, 0), std::bad_alloc);
}

BOOST_AUTO_TEST_CASE( stl_deque )
{
    typedef int data_type;
    typedef lazy::memory::buffer_allocator<data_type> allocator_type;
    typedef std::deque<data_type, allocator_type> deque_type;

    const size_t buffer_size = 32 * 1024;
    data_type buffer[buffer_size];
    allocator_type allocator(buffer, sizeof(buffer));
    deque_type d(allocator);
    BOOST_REQUIRE_NO_THROW(d.push_front(0));
    BOOST_REQUIRE_NO_THROW(d.push_back(1));
    BOOST_REQUIRE_EQUAL(d[0], 0);
    BOOST_REQUIRE_EQUAL(d[1], 1);
    BOOST_REQUIRE_THROW(d.resize(buffer_size), std::bad_alloc);
}

BOOST_AUTO_TEST_CASE( stl_queue )
{
    typedef int data_type;
    typedef lazy::memory::buffer_allocator<data_type> allocator_type;
    typedef std::deque<data_type, allocator_type> deque_type;
    typedef std::queue<data_type, deque_type> queue_type;

    const size_t buffer_size = 32 * 1024;
    data_type buffer[buffer_size];
    allocator_type allocator(buffer, sizeof(buffer));
    deque_type d(allocator);
    queue_type q(d);
    BOOST_REQUIRE_NO_THROW(q.push(1));
    BOOST_REQUIRE_NO_THROW(q.push(2));
    BOOST_REQUIRE_EQUAL(q.front(), 1);
    BOOST_REQUIRE_EQUAL(q.back(), 2);
    BOOST_REQUIRE_NO_THROW(q.pop());
    BOOST_REQUIRE_EQUAL(q.front(), 2);
    BOOST_REQUIRE_EQUAL(q.back(), 2);
}

BOOST_AUTO_TEST_CASE( stl_map )
{
    typedef int key_type;
    typedef int data_type;
    typedef std::pair<const key_type, data_type> value_type;
    typedef lazy::memory::buffer_allocator<value_type> allocator_type;
    typedef std::map<key_type, data_type, std::less<key_type>, allocator_type> map_type;

    // DISCLAIMER:
    // Again, making the buffer "big enough" since I don't know the allocation behavior
    // of std::map yet.
    const size_t buffer_size = 128 * 1024;
    char buffer[buffer_size];
    allocator_type allocator(buffer, sizeof(buffer));
    std::less<key_type> cmp;
    map_type m(cmp, allocator);
    map_type::value_type value(1, 1);
    BOOST_REQUIRE_NO_THROW(m.insert(value));
}

BOOST_AUTO_TEST_CASE( tr1_unordered_map )
{
    typedef int key_type;
    typedef int data_type;
    typedef std::pair<const key_type, data_type> value_type;
    typedef lazy::memory::buffer_allocator<value_type> allocator_type;
    typedef std::tr1::unordered_map<key_type, data_type, std::hash<key_type>, std::equal_to<key_type>, allocator_type> map_type;

    // DISCLAIMER:
    // You know it.
    const size_t buffer_size = 128 * 1024;
    char buffer[buffer_size];
    allocator_type allocator(buffer, sizeof(buffer));
    std::hash<key_type> hasher;
    std::equal_to<key_type> cmp;
    map_type m(10, hasher, cmp, allocator);
    map_type::value_type value(1, 1);
    BOOST_REQUIRE_NO_THROW(m.insert(value));
}

BOOST_AUTO_TEST_CASE( stl_unordered_multimap )
{
    typedef int key_type;
    typedef int data_type;
    typedef std::pair<const key_type, data_type> value_type;
    typedef lazy::memory::buffer_allocator<value_type> allocator_type;
    typedef std::tr1::unordered_multimap<key_type, data_type, std::hash<key_type>, std::equal_to<key_type>, allocator_type> map_type;

    // DISCLAIMER:
    // You know it.
    const size_t buffer_size = 128 * 1024;
    char buffer[buffer_size];
    allocator_type allocator(buffer, sizeof(buffer));
    std::hash<key_type> hasher;
    std::equal_to<key_type> cmp;
    map_type m(10, hasher, cmp, allocator);
    map_type::value_type value(1, 1);
    BOOST_REQUIRE_NO_THROW(m.insert(value));
    BOOST_REQUIRE_NO_THROW(m.insert(value));
}

BOOST_AUTO_TEST_CASE( tr1_unordered_set )
{
    typedef int key_type;
    typedef lazy::memory::buffer_allocator<key_type> allocator_type;
    typedef std::tr1::unordered_set<key_type, std::hash<key_type>, std::equal_to<key_type>, allocator_type> set_type;

    // DISCLAIMER:
    // You know it.
    const size_t buffer_size = 128 * 1024;
    char buffer[buffer_size];
    allocator_type allocator(buffer, sizeof(buffer));
    std::hash<key_type> hasher;
    std::equal_to<key_type> cmp;
    set_type m(10, hasher, cmp, allocator);
    set_type::key_type key(1);
    BOOST_REQUIRE_NO_THROW(m.insert(key));
}

BOOST_AUTO_TEST_CASE( stl_unordered_multiset )
{
    typedef int key_type;
    typedef lazy::memory::buffer_allocator<key_type> allocator_type;
    typedef std::tr1::unordered_multiset<key_type, std::hash<key_type>, std::equal_to<key_type>, allocator_type> set_type;

    // DISCLAIMER:
    // You know it.
    const size_t buffer_size = 128 * 1024;
    char buffer[buffer_size];
    allocator_type allocator(buffer, sizeof(buffer));
    std::hash<key_type> hasher;
    std::equal_to<key_type> cmp;
    set_type m(10, hasher, cmp, allocator);
    set_type::key_type key(1);
    BOOST_REQUIRE_NO_THROW(m.insert(key));
    BOOST_REQUIRE_NO_THROW(m.insert(key));
}

// EOF
