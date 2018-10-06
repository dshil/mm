#include <stdio.h>

#include "heap_allocator.h"
#include "catch2/catch.hpp"

class Foo {
public:
    explicit Foo(size_t num) : num_(num) {
    }

    Foo() {}

    ~Foo() {
    }
private:
    size_t num_;
};

struct Bar {
    int bar;
};

TEST_CASE("alloc") {
    core::HeapAllocator allocator;

    Foo* p = MM_ALLOC(Foo, allocator);
    REQUIRE(p);
    MM_FREE(p, allocator);
}

TEST_CASE("alloc-array") {
    core::HeapAllocator allocator;

    Foo* foo = MM_ALLOC_ARRAY(Foo, 3, allocator);
    REQUIRE(foo);
    MM_FREE_ARRAY(foo, allocator);

    Bar* bar = MM_ALLOC_ARRAY(Bar, 10, allocator);
    REQUIRE(bar);
    MM_FREE_ARRAY(bar, allocator);
}
