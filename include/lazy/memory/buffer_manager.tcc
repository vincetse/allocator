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
#ifndef __LAZY_BUFFER_MANAGER_TCC__
#define __LAZY_BUFFER_MANAGER_TCC__

#include <bits/functexcept.h>

namespace lazy {
namespace memory {
////////////////////////////////////////////////////////////////////////////////
// buffer_manager
////////////////////////////////////////////////////////////////////////////////

// when you copy, you need to move the cursor in the buffer forward and give up some memory
inline buffer_manager::buffer_manager(void* buffer, buffer_manager::size_type buffer_size) :
    m_buffer(buffer),
    m_buffer_size(buffer_size),
    m_bytes_allocated(0)
{
    // NOP
}

inline buffer_manager::buffer_manager(const buffer_manager& source) :
    m_buffer(source.m_buffer),
    m_buffer_size(source.m_buffer_size),
    m_bytes_allocated(source.m_bytes_allocated)
{
    // NOP
}

inline buffer_manager::size_type buffer_manager::buffer_size() const
{
    return m_buffer_size;
}

inline buffer_manager::size_type buffer_manager::available() const
{
    return (m_buffer_size - m_bytes_allocated);
}

inline void* buffer_manager::allocate(buffer_manager::size_type n)
{
    const size_type remains = available();
    if (remains < n) {
        std::__throw_bad_alloc();
    }   
    void* cursor = static_cast<void*>(static_cast<char*>(m_buffer) + m_bytes_allocated);
    m_bytes_allocated += n;
    return cursor;
}

} // namespace memory
} // namespace lazy

#endif // __LAZY_BUFFER_MANAGER_TCC__
