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

template<typename T>
inline buffer_allocator<T>::buffer_allocator(typename buffer_allocator<T>::pointer buffer,
        typename buffer_allocator<T>::size_type buffer_size) throw() :
    m_buffer(buffer),
    m_buffer_size(buffer_size),
    m_nallocated(0)
{

}

template<typename T>
inline buffer_allocator<T>::buffer_allocator(const buffer_allocator<T>& alloc) throw() :
    m_buffer(alloc.m_buffer),
    m_buffer_size(alloc.m_buffer_size),
    m_nallocated(alloc.m_nallocated)
{
    // NOP
}

template<typename T>
inline buffer_allocator<T>::~buffer_allocator() throw()
{
    // NOP
}

template<typename T>
inline typename buffer_allocator<T>::pointer buffer_allocator<T>::address(
    typename buffer_allocator<T>::reference x) const
{
    return &x;
}

template<typename T>
inline typename buffer_allocator<T>::const_pointer buffer_allocator<T>::address(
    typename buffer_allocator<T>::const_reference x) const
{
    return &x;
}

template<typename T>
inline typename buffer_allocator<T>::pointer buffer_allocator<T>::allocate(
    typename buffer_allocator<T>::size_type n, typename buffer_allocator<T>::const_pointer)
{
    const size_type nallocated = m_nallocated + n;
    if (nallocated > max_size()) {
        std::__throw_bad_alloc();
    }
    m_nallocated = nallocated;
    pointer cursor = static_cast<pointer>(m_buffer + (m_nallocated * sizeof(value_type)));
    return cursor;
}

template<typename T>
inline void buffer_allocator<T>::deallocate(typename buffer_allocator<T>::pointer p,
    typename buffer_allocator<T>::size_type n)
{
    // Do not attempt to return memory buffer cos we are a high-watermark allocator.
    // NOP?  TODO is it really noop or do we have to call the destructor?
}

template<typename T>
inline typename buffer_allocator<T>::size_type buffer_allocator<T>::max_size() const throw()
{
    return static_cast<size_type>(m_buffer_size / sizeof(T));
}

template<typename T>
inline void buffer_allocator<T>::construct(typename buffer_allocator<T>::pointer p,
   const  typename buffer_allocator<T>::value_type& x)
{
    ::new ((void*)p) value_type(x);
}

template<typename T>
inline void buffer_allocator<T>::destroy(typename buffer_allocator<T>::pointer p)
{
    p->~T();
}

} // namespace lazy

#endif // __LAZY_BUFFER_ALLOCATOR_INL__
