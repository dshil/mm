#include <stdio.h>
#include <assert.h>

#include "include/heap_allocator.h"

class Foo {
public:
    ~Foo() {
    }
    Foo(size_t num) : num_(num) {
    }
    Foo() {}
private:
    size_t num_;
};

int main() {
    core::HeapAllocator allocator;
    {
        Foo *p = MM_ALLOC(Foo, allocator);
        assert(p);
        MM_FREE(p, allocator);
    }
    {
        Foo *p = MM_ALLOC_ARRAY(Foo, 3, allocator);
        assert(p);
        MM_FREE_ARRAY(p, allocator);
    }
}