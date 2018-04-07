#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>

#include "memory/pool.h"

int main()
{
	{
		void *p = mm_pool_get();
		assert(p);
	}
	{
		void *p = mm_pool_get();
		assert(p);
		mm_pool_put(p);
	}
	{
		char *ptr = (char *)malloc(BUFSIZ);
		assert(ptr);

		pool_config_t config = {
			.begin = ptr,
			.pool_size = BUFSIZ,
			.elem_size = BUFSIZ / 128,
			.alloc_type = MALLOC_ALLOCATOR,
		};
		assert(mm_initialize_pool(&config) != -1);

		void *p = mm_pool_get();
		assert(p);
		mm_pool_put(p);
	}
	{
		char ptr[BUFSIZ];
		pool_config_t config = {
			.begin = ptr,
			.pool_size = BUFSIZ,
			.elem_size = BUFSIZ / 128,
		};
		assert(mm_initialize_pool(&config) != -1);

		void *p = mm_pool_get();
		assert(p);
		mm_pool_put(p);
	}
}
