#include <unistd.h>

#include "heap_allocator.h"

namespace core {

HeapAllocator::~HeapAllocator() {
}

void *HeapAllocator::alloc(size_t size) {
    return malloc(size);
}

void HeapAllocator::dealloc(void *ptr) {
    free(ptr);
}

} // namespace core
