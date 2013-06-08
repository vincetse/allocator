AM_TESTS_ENVIRONMENT="\valgrind --show-reachable=yes --leak-check=full"

check_PROGRAMS= \
    buffer_allocator_test \
    buffer_allocator_container_test

buffer_allocator_test_SOURCES= \
    buffer_allocator_test.cpp

buffer_allocator_container_test_SOURCES= \
	buffer_allocator_container_test.cpp

LDADD= \
    -lboost_unit_test_framework

TESTS= \
    $(check_PROGRAMS)
