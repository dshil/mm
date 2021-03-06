#include "heap_allocator.h"

extern "C" {
#include "memory/mm.h"
}

namespace core {

HeapAllocator::~HeapAllocator() {
}

void *HeapAllocator::alloc(size_t size) {
    return mmalloc(size);
}

void HeapAllocator::dealloc(void *ptr) {
    mfree(ptr);
}

} // namespace core
