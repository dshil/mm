#ifndef MM_HEAP_ALLOCATOR_H_
#define MM_HEAP_ALLOCATOR_H_

#include "iallocator.h"

namespace core {

class HeapAllocator : public IAllocator {
public:
    ~HeapAllocator();

    virtual void *alloc(size_t size);
    virtual void dealloc(void *ptr);
};

} // namespace core

#endif // MM_HEAP_ALLOCATOR_H_
