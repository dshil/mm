#include <stdio.h>

extern "C" {
#include "memory/kr/kr.h"
#include "memory/mm.h"
} // extern "C"

#include "catch2/catch.hpp"

TEST_CASE("free_arbitrary") {
    char pool[BUFSIZ];
    char* ptr = pool;
    mfree_arbitrary(static_cast<void*>(ptr), BUFSIZ);

    int* p = static_cast<int*>(mmalloc(10 * sizeof(int)));
    REQUIRE(p);
    *p = 1000;
    REQUIRE(*p == 1000);
}
