#include <stdio.h>

extern "C" {
#include "memory/mm.h"
}

#include "catch2/catch.hpp"

TEST_CASE("alloc") {
    int* ptr = static_cast<int*>(mmalloc(10 * sizeof(int)));
    REQUIRE(ptr);
    mfree(ptr);
}

TEST_CASE("alloc-set") {
    int* ptr = static_cast<int*>(mmalloc(10 * sizeof(int)));
    *ptr = 100;
    REQUIRE(*ptr == 100);
    mfree(ptr);
}

TEST_CASE("alloc-big-chunk") {
    long* ptr = static_cast<long*>(mmalloc(BUFSIZ * 128 * sizeof(long)));
    *ptr = 100;
    REQUIRE(*ptr == 100);
    mfree(ptr);
}

TEST_CASE("free-null") {
    mfree(NULL);
}

TEST_CASE("zero-value-alloc") {
    int* ptr = static_cast<int*>(mcalloc(10, sizeof(int)));
    REQUIRE(ptr);
    REQUIRE(*ptr == 0);
    mfree(ptr);
}

TEST_CASE("zero-value-alloc-big-chunk") {
    int* ptr = static_cast<int*>(mcalloc(BUFSIZ * BUFSIZ, sizeof(int)));
    REQUIRE(ptr);
    REQUIRE(*ptr == 0);
    mfree(ptr);
}

TEST_CASE("realloc") {
    int* ptr = static_cast<int*>(mrealloc(NULL, 10 * sizeof(int)));
    REQUIRE(ptr);
    mfree(ptr);
}

TEST_CASE("alloc-realloc") {
    int* ptr = static_cast<int*>(mmalloc(sizeof(int)));
    *ptr = 100;

    ptr = static_cast<int*>(mrealloc(ptr, 200 * sizeof(int)));
    *(ptr + 1) = 200;

    REQUIRE(*ptr == 100);
    REQUIRE(*(ptr + 1) == 200);

    mfree(ptr);
}
