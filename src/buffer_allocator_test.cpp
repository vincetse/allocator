#include "lazy/memory/buffer_allocator.hpp"
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE BufferAllocatorTest
#include <boost/test/unit_test.hpp>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#include <utility>
#include <functional>
#include <algorithm>

BOOST_AUTO_TEST_CASE( max_size_test )
{
    typedef int data_type;
    typedef lazy::memory::buffer_allocator<data_type> allocator_type;

    const size_t num_objects = 2;
    data_type buffer[num_objects];
    allocator_type allocator(buffer, sizeof(buffer));
    BOOST_REQUIRE_EQUAL(allocator.max_size(), num_objects);
    data_type* a = 0;
    BOOST_REQUIRE_NO_THROW(a = allocator.allocate(1));
    BOOST_REQUIRE(a);
    a = 0;
    BOOST_REQUIRE_NO_THROW(a = allocator.allocate(1));
    BOOST_REQUIRE(a);
    a = 0;
    BOOST_REQUIRE_THROW(allocator.allocate(1), std::bad_alloc);
}


BOOST_AUTO_TEST_CASE( address )
{
    typedef int data_type;
    typedef lazy::memory::buffer_allocator<data_type> allocator_type;

    const size_t num_objects = 2;
    data_type buffer[num_objects];
    allocator_type allocator(buffer, sizeof(buffer));

    BOOST_REQUIRE_EQUAL(allocator.max_size(), num_objects);
    data_type* a = 0;
    BOOST_REQUIRE_NO_THROW(a = allocator.allocate(1));
    BOOST_REQUIRE(a);

    const data_type& b = *a;
    BOOST_REQUIRE_EQUAL(a, allocator.address(b));

    data_type& c = *a;
    BOOST_REQUIRE_EQUAL(a, allocator.address(c));
}

BOOST_AUTO_TEST_CASE( buffer_size_is_honored )
{
    typedef int data_type;
    typedef lazy::memory::buffer_allocator<data_type> allocator_type;

    const size_t buffer_size = 0;
    const size_t num_objects = 2;
    data_type buffer[num_objects];
    allocator_type allocator(buffer, buffer_size);
    BOOST_REQUIRE_EQUAL(allocator.max_size(), 0);
    BOOST_REQUIRE_THROW(allocator.allocate(1), std::bad_alloc);
}

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
