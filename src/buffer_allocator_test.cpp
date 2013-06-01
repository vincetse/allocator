#include "lazy/buffer_allocator.hpp"
#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MAIN
#define BOOST_TEST_MODULE BufferAllocatorTest
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE( max_size_test )
{
    typedef int data_type;
    typedef lazy::buffer_allocator<data_type> allocator_type;

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

// EOF
