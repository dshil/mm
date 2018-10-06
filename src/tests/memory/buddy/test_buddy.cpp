#include <stdio.h>
#include <stdlib.h>

extern "C" {
#include "memory/mm.h"
#include "memory/buddy/buddy.h"
} // extern "C"

#include "catch2/catch.hpp"

TEST_CASE("mmalloc") {
    const size_t size = 100;
    char *p = static_cast<char*>(mmalloc(size));
    REQUIRE(p);
    mfree_bytes(p, size);
}
