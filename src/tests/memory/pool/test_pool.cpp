#include <stdio.h>

extern "C" {
#include "memory/pool/pool.h"
} // extern "C"

#include "catch2/catch.hpp"

TEST_CASE("get") {
	REQUIRE(mm_pool_get());
}

TEST_CASE("get-put") {
	void *p = mm_pool_get();
	REQUIRE(p);
	mm_pool_put(p);
}

TEST_CASE("malloc-allocator") {
	char *ptr = static_cast<char*>(malloc(BUFSIZ));
	REQUIRE(ptr);

	pool_config_t config = {
		.begin = ptr,
		.pool_size = BUFSIZ,
		.elem_size = BUFSIZ / 128,
		.alloc_type = MALLOC_ALLOCATOR,
	};
	REQUIRE(mm_initialize_pool(&config) != -1);

	void *p = mm_pool_get();
	REQUIRE(p);
	mm_pool_put(p);
}

TEST_CASE("static-allocator") {
	char ptr[BUFSIZ];
	pool_config_t config = {
		.begin = ptr,
		.pool_size = BUFSIZ,
		.elem_size = BUFSIZ / 128,
	};
	REQUIRE(mm_initialize_pool(&config) != -1);

	void *p = mm_pool_get();
	REQUIRE(p);
	mm_pool_put(p);
}
