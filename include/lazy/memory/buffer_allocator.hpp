// The MIT License (MIT)
// 
// Copyright (c) 2013 Vince Tse
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
#ifndef __LAZY_BUFFER_ALLOCATOR_HPP__
#define __LAZY_BUFFER_ALLOCATOR_HPP__

#include <cstdlib>
#include <lazy/memory/buffer_manager.hpp>

namespace lazy {
namespace memory {

// This is a memory allocator that uses stack memory, and then falls back to the heap
// when the stack memory is exhausted.  This is a high-watermark allocator that does not reuse
// freed blocks, so you return memory letting your objects go out of scope.
// This class was created by a desire to stop people from reinventing the wheel reimplementing
// STL classes cos they don't like the way the STL uses memory, as well as by John Wellbelove's
// fixed_allocator.
// http://www.codeguru.com/cpp/article.php/c18503/C-Programming-Stack-Allocators-for-STL-Containers.htm
template <typename T>
class buffer_allocator
{
public:
    typedef T value_type;
    typedef buffer_manager::size_type size_type;
    typedef std::ptrdiff_t difference_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;

    // \brief I don't know what rebind is for.
    template <typename U>
    struct rebind
    {
        typedef buffer_allocator<U> other;
    };

    // \brief default ctor that doesn't do anything meaningful.  Don't use it.
    buffer_allocator() throw();

    // \brief ctor
    // \param[in] buffer  the array to allocate memory from
    // \param[in] buffer_size  the size of the array in bytes
    buffer_allocator(void* buffer, size_type buffer_size) throw();

    // \brief copy ctor
    buffer_allocator(const buffer_allocator&) throw();

    // \brief rebind ctor
    template <typename U>
    buffer_allocator(const buffer_allocator<U>& alloc) throw();

    // \brief dtor
    ~buffer_allocator() throw();

    // \brief Returns the address of the supplied object
    pointer address(reference x) const;

    // \brief Returns the address of the supplied object
    const_pointer address(const_reference x) const;

    // \brief Allocates the memory for 'n' objects. Pointer cp is ignored
    // \param[in] n  the number of objects to allocate
    // \param[in] cp unused
    pointer allocate(size_type n, const_pointer cp = 0);

    // \brief Releases the previously allocated resource
    void deallocate(pointer p, size_type n);

    // \brief Returns the maximum size that the container may grow to
    size_type max_size() const throw();

    // \brief Constructs an instance of the object
    void construct(pointer p, const value_type& x);

    // \brief Releases the resources owned by the object pointed to
    void destroy(pointer p);

    // \brief gets the buffer_manager
    buffer_manager& get_buffer_manager() const;

protected:
    // \brief reference to the buffer manager that is actively being used
    buffer_manager& m_buffer_manager;

private:
    // \brief the space for a buffer_manager object if this object isn't copy-constructed
    buffer_manager m_buffer_manager_storage;
};

// these are a necessary evil cos some STL classes (e.g. std::basic_string uses them)
template<typename T, typename U>
bool operator==(const buffer_allocator<T>&, const buffer_allocator<U>&);

template<typename T>
bool operator==(const buffer_allocator<T>&, const buffer_allocator<T>&);

template<typename T, typename U>
bool operator!=(const buffer_allocator<T>&, const buffer_allocator<U>&);

template<typename T>
bool operator!=(const buffer_allocator<T>&, const buffer_allocator<T>&);

} // namespace memory
} // namespace lazy

#include "buffer_allocator.inl"

#endif // __LAZY_BUFFER_ALLOCATOR_HPP__
