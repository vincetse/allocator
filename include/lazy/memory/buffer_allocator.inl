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
#ifndef __LAZY_BUFFER_ALLOCATOR_INL__
#define __LAZY_BUFFER_ALLOCATOR_INL__

//#include <cstddef>
#include <bits/functexcept.h>

namespace lazy {
namespace memory {
////////////////////////////////////////////////////////////////////////////////
// buffer_allocator
////////////////////////////////////////////////////////////////////////////////
template <typename T>
inline buffer_allocator<T>::buffer_allocator() throw() :
    m_buffer_manager(m_buffer_manager_storage),
    m_buffer_manager_storage(0, 0)
{
    // NOP
}

template <typename T>
inline buffer_allocator<T>::buffer_allocator(void* buffer,
        typename buffer_allocator<T>::size_type buffer_size) throw() :
    m_buffer_manager(m_buffer_manager_storage),
    m_buffer_manager_storage(buffer, buffer_size)
{
    // NOP
}

template <typename T>
inline buffer_allocator<T>::buffer_allocator(const buffer_allocator<T>& alloc) throw() :
    m_buffer_manager(alloc.get_buffer_manager()),
    m_buffer_manager_storage(0, 0)
{
    // NOP
}

template <typename T>
template <typename U>
inline buffer_allocator<T>::buffer_allocator(const buffer_allocator<U>& alloc) throw() :
    m_buffer_manager(alloc.get_buffer_manager()),
    m_buffer_manager_storage(0, 0)
{
    // NOP
}

template <typename T>
inline buffer_allocator<T>::~buffer_allocator() throw()
{
    // NOP
}

template <typename T>
inline typename buffer_allocator<T>::pointer buffer_allocator<T>::address(
    typename buffer_allocator<T>::reference x) const
{
    return &x;
}

template <typename T>
inline typename buffer_allocator<T>::const_pointer buffer_allocator<T>::address(
    typename buffer_allocator<T>::const_reference x) const
{
    return &x;
}

template <typename T>
inline typename buffer_allocator<T>::pointer buffer_allocator<T>::allocate(
    typename buffer_allocator<T>::size_type n, typename buffer_allocator<T>::const_pointer)
{
    // assuming that sizeof() rounds up, so m_bytes_allocated will always be in the
    // multiple to ensure alignment.
    const size_type bytes = n * sizeof(T);
    pointer const cursor = static_cast<pointer>(m_buffer_manager.allocate(bytes));
    return cursor;
}

template <typename T>
inline void buffer_allocator<T>::deallocate(typename buffer_allocator<T>::pointer p,
    typename buffer_allocator<T>::size_type n)
{
    // Do not attempt to return memory buffer cos we are a high-watermark allocator.
    // NOP?  TODO is it really noop or do we have to call the destructor?
}

template <typename T>
inline typename buffer_allocator<T>::size_type buffer_allocator<T>::max_size() const throw()
{
    return static_cast<size_type>(m_buffer_manager.buffer_size() / sizeof(T));
}

template <typename T>
inline void buffer_allocator<T>::construct(typename buffer_allocator<T>::pointer p,
   const  typename buffer_allocator<T>::value_type& x)
{
    ::new ((void*)p) value_type(x);
}

template <typename T>
inline void buffer_allocator<T>::destroy(typename buffer_allocator<T>::pointer p)
{
    p->~T();
}

template <typename T>
inline buffer_manager& buffer_allocator<T>::get_buffer_manager() const
{
    return m_buffer_manager;
}

////////////////////////////////////////////////////////////////////////////////
// operators
////////////////////////////////////////////////////////////////////////////////
template<typename T, typename U>
inline bool operator==(const buffer_allocator<T>&, const buffer_allocator<U>&)
{
    // doing what std::allocator does
    return true;
}

template<typename T>
inline bool operator==(const buffer_allocator<T>&, const buffer_allocator<T>&)
{
    // doing what std::allocator does
    return true;
}

template<typename T, typename U>
inline bool operator!=(const buffer_allocator<T>&, const buffer_allocator<U>&)
{
    // doing what std::allocator does
    return false;
}

template<typename T>
inline bool operator!=(const buffer_allocator<T>&, const buffer_allocator<T>&)
{
    // doing what std::allocator does
    return false;
}

} // namespace memory
} // namespace lazy

#endif // __LAZY_BUFFER_ALLOCATOR_INL__
