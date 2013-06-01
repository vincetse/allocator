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

namespace lazy {

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
private:
    // \brief default ctor that we don't use cos you need to tell this allocator how much memory to allocate
    buffer_allocator() throw();

public:
    typedef T value_type;
    typedef std::size_t size_type;
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

    // \brief ctor
    // \param[in] 
    buffer_allocator(T* buffer, size_type buffer_size) throw();

    // \brief copy ctor
    buffer_allocator(const buffer_allocator<T>&) throw();

    // \brief dtor
    ~buffer_allocator() throw();

    // \brief Returns the address of the supplied object
    pointer address(reference x) const;

    // \brief Returns the address of the supplied object
    const_pointer address(const_reference x) const;

    // \brief Allocates the memory for 'n' objects. Pointer cp is ignored
    pointer allocate(size_type n, const_pointer cp = 0);

    // \brief Releases the previously allocated resource
    void deallocate(pointer p, size_type n);

    // \brief Returns the maximum size that the container may grow to
    size_type max_size() const throw();

    // \brief Constructs an instance of the object
    void construct(pointer p, const value_type& x);

    // \brief Releases the resources owned by the object pointed to
    void destroy(pointer p);

protected:
    // \brief block of memory
    pointer const m_buffer;

    // \brief buffer size
    const size_type m_buffer_size;

    // \brief the number of object allocated
    size_type m_nallocated;
};

} // namespace lazy

#include "buffer_allocator.inl"

#endif // __LAZY_BUFFER_ALLOCATOR_HPP__
