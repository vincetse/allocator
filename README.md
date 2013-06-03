memory-allocators
=================

A set of memory allocators for use with the C++ Standard Template Library.

buffer_allocator
----------------

This allocator was inspired by [John Wellbelove](http://www.codeguru.com/member.php/John+Wellbelove/)'s article, [Stack Allocators for STL Containers](http://www.codeguru.com/cpp/article.php/c18503/C-Programming-Stack-Allocators-for-STL-Containers.htm), and is an attempt to offer an alternative solution to John's implementation (which needs a few tweaks to run on Linux).

### Before You Start

Memory allocators are a slightly advanced concept in C++, so I would highly recommend you do not try to use this library if you are not familiar with them.  There are simply too many ways you can shoot yourself in the foot without understanding why.  I gave some background on how I got into this topic in [my blog post](http://thelazyenginerd.com/post/52004241212/stack-allocators-for-stl-containers) about John's article, which makes me think I have a slight clue what I am doing here.

If you are interested in improving the memory management of your application, I recommend you try a new malloc library first before messing with memory allocators.  The 2 that I have experience with are:

1. [jemalloc](http://www.canonware.com/jemalloc/)
2. [tcmalloc](https://code.google.com/p/gperftools/)

[Nedmalloc](http://www.nedprod.com/programs/portable/nedmalloc/) seems to be another possibility, though I do not have any experience with it.

### Design

This is a high-water mark allocator that lets you define the amount of memory available to the allocator by creating a buffer (either on the stack or on the heap) and then passing a pointer to the buffer to the allocator during construction.  The amount of memory available cannot be increased during the lifetime of the allocator, so you will have to allocate as much as you expect to need when you start.  The nice thing about this approach is that your application's memory usage will not grow unexpectedly over time without you knowing since you will have to change the amount available.  The bad thing is that you will have to change it manually since memory is not allocated dynamically.

This section needs to be expanded.  Any volunteers?

| Class                          | Description |
|--------------------------------|-------------|
| `lazy::memory::buffer_manager`   | This is a simple wrapper class that cuts up a byte array of memory allocated from the stack or heap and make it available to classes allocating memory with `lazy::memory::buffer_allocator. |
| `lazy::memory::buffer_allocator` | A `std::allocator`-compatible class that can be used STL or STL-like containers. |


### Pre-requisites

This library was developed in an [Ubuntu Precise chroot](https://github.com/thelazyenginerd/vroot) on a Debian Squeeze 6.0.7 virtual machine.  It may work in other environments, but I have not had the chance to test it, so please let me know if it works in other environments, or send me pull requests if you have patches to make it work in other environments.

1. G++ 4.6.3
2. GNU Autoconf 2.68
3. GNU Automake 1.11.3
4. GNU Libtool 2.4.2
5. GNU Make 3.81
6. Boost 1.53.0

\#2-6 are used for unit testing and installing the files to the include directory.  You can always skip unit tests and manually copy the files to your include directory.

### Examples

I have tossed together some examples to help get you started.  These are fairly basic since they are copied from the [unit tests](/blob/master/src/buffer_allocator_container_test.cpp).

#### std::vector

An example demonstrating how to use buffer_allocator with a `std::vector`.

    typedef int data_type;
    typedef lazy::memory::buffer_allocator<data_type> allocator_type;
    
    // create the buffer array where memory will be allocated from
    const size_t num_objects = 3;
    data_type buffer[num_objects];
    allocator_type allocator(buffer, sizeof(buffer));
    
    // now declare a std::vector to allocate memory from the buffer, which in
    // this example lives on the stack (as opposed to the heap).
    std::vector<data_type, allocator_type> vec(allocator)
    vec.push_back(1)
    vec.push_back(2);

#### std::basic_string

We have to use `std::basic_string` instead of `std::string` cos the latter does not have a memory allocator template parameter.

    typedef char data_type;
    typedef lazy::memory::buffer_allocator<data_type> allocator_type;
    typedef std::basic_string<data_type, std::char_traits<data_type>, allocator_type> string_type;
    
    // allocate a buffer to hold the chars
    const size_t buffer_size = 32 * 1024;
    data_type buffer[buffer_size];
    allocator_type allocator(buffer, sizeof(buffer));
    
    // now use the buffer_allocator with a std::basic_string
    string_type str(allocator);
    str = "Hello, world!";

Note that `std::string` is a kind of wrapper which reference-counts the underlying memory and shares it across multiple `std::string` objects and only allocates member to copy the data on write.  Look in the unit tests for more details.

#### std::list

    typedef int data_type;
    typedef lazy::memory::buffer_allocator<data_type> allocator_type;
    typedef std::list<data_type, allocator_type> list_type;

    const size_t buffer_size = 32 * 1024;
    data_type buffer[buffer_size];
    allocator_type allocator(buffer, sizeof(buffer));
    list_type l(allocator);
    l.push_back(1));
    l.push_front(2);

#### std::deque

    typedef int data_type;
    typedef lazy::memory::buffer_allocator<data_type> allocator_type;
    typedef std::deque<data_type, allocator_type> deque_type;

    const size_t buffer_size = 32 * 1024;
    data_type buffer[buffer_size];
    allocator_type allocator(buffer, sizeof(buffer));
    deque_type d(allocator);
    d.push_front(0));
    d.push_back(1);

#### std::queue

    typedef int data_type;
    typedef lazy::memory::buffer_allocator<data_type> allocator_type;
    typedef std::deque<data_type, allocator_type> deque_type;
    typedef std::queue<data_type, deque_type> queue_type;

    const size_t buffer_size = 32 * 1024;
    data_type buffer[buffer_size];
    allocator_type allocator(buffer, sizeof(buffer));
    deque_type d(allocator);
    queue_type q(d);
    q.push(1);

#### std::map

Writing the example with `std::map` was very enlightening since it taught me about [the rebinding that happens inside the class](http://stackoverflow.com/questions/15488527/rebinding-in-a-custom-stl-allocator-with-pre-allocated-block).  The lesson here is that your buffer size should be somewhat bigger than what you expect to need for your key/value pairs cos memory is needed for the red-black tree that `std::map` is implemented with.

    typedef int key_type;
    typedef int data_type;
    typedef std::pair<const key_type, data_type> value_type;
    typedef lazy::memory::buffer_allocator<value_type> allocator_type;
    typedef std::map<key_type, data_type, std::less<key_type>, allocator_type> map_type;

    const size_t buffer_size = 128 * 1024;
    char buffer[buffer_size];
    allocator_type allocator(buffer, sizeof(buffer));
    std::less<key_type> cmp;
    map_type m(cmp, allocator);
    map_type::value_type value(1, 1); 
    m.insert(value);

#### std::tr1::unordered_map

This example shows how to use `buffer_allocator` with `std::tr1::unordered_map`.  The old-style `__gnu_cxx::hash_map` cannot be used since it does not have a suitable constructor that takes an allocator.

    typedef int key_type;
    typedef int data_type;
    typedef std::pair<const key_type, data_type> value_type;
    typedef lazy::memory::buffer_allocator<value_type> allocator_type;
    typedef std::tr1::unordered_map<key_type, data_type, std::hash<key_type>, std::equal_to<key_type>, allocator_type> map_type;
    
    const size_t buffer_size = 128 * 1024;
    char buffer[buffer_size];
    allocator_type allocator(buffer, sizeof(buffer));
    std::hash<key_type> hasher;
    std::equal_to<key_type> cmp;
    map_type m(10, hasher, cmp, allocator);
    map_type::value_type value(1, 1);
    m.insert(value);

#### std::tr1::unordered_set

    typedef int key_type;
    typedef lazy::memory::buffer_allocator<key_type> allocator_type;
    typedef std::tr1::unordered_set<key_type, std::hash<key_type>, std::equal_to<key_type>, allocator_type> set_type;

    const size_t buffer_size = 128 * 1024;
    char buffer[buffer_size];
    allocator_type allocator(buffer, sizeof(buffer));
    std::hash<key_type> hasher;
    std::equal_to<key_type> cmp;
    set_type m(10, hasher, cmp, allocator);
    set_type::key_type key(1);
    m.insert(key);

