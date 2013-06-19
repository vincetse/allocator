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
#ifndef __LAZY_BUFFER_MANAGER_H__
#define __LAZY_BUFFER_MANAGER_H__

#include <cstdlib>

namespace lazy {
namespace memory {

// \brief manages the allocation of memory from the buffer
class buffer_manager
{
public:
    typedef std::size_t size_type;

    // \brief ctor
    // \param[in] buffer  pointer to the buffer to use for allocation
    // \param[in] buffer_size  size of the buffer.  make sure they match.
    buffer_manager(void* buffer, size_type buffer_size);

    // \brief copy ctor
    // \param[in] source  the source
    buffer_manager(const buffer_manager& source);

    // \brief the buffer size in this class
    size_type buffer_size() const;

    // \brief the amount of space remaining in buffer
    size_type available() const;

    // \brief allocates a chunk of memory of requested size
    // \param[in] n   size of chunk in bytes
    //
    // TODO are byte sizes in multiples of 4 or 8?
    void* allocate(size_type n);

protected:
    // \brief block of memory
    void* const m_buffer;

    // \brief buffer size
    const size_type m_buffer_size;

    // \brief the number of bytes allocated
    size_type m_bytes_allocated;
};

} // namespace memory
} // namespace lazy

#include "buffer_manager.tcc"

#endif // __LAZY_BUFFER_MANAGER_H__
