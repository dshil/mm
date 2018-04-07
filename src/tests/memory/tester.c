#include <stdio.h>
#include <assert.h>

#include "memory/mm.h"

void test_main(void)
{
	{
		int *ptr = (int *)mmalloc(sizeof(int)*10);
		assert(ptr != NULL);
		mfree(ptr);
	}
	{
		int *ptr = (int *)mmalloc(sizeof(int)*10);

		*ptr = 100;
		assert(*ptr == 100);

		mfree(ptr);
	}
	{
		long *ptr = (long *)mmalloc(BUFSIZ * 128 * sizeof(long));

		*ptr = 100;
		assert(*ptr == 100);

		mfree(ptr);
	}
	{
		mfree(NULL);
	}
	{
		int *ptr = (int *)mcalloc(10, sizeof(int));

		assert(ptr != NULL);
		assert(*ptr == 0);

		mfree(ptr);
	}
	{
		int *ptr = (int *)mcalloc(BUFSIZ * BUFSIZ, sizeof(int));

		assert(ptr != NULL);
		assert(*ptr == 0);

		mfree(ptr);
	}
	{
		int *ptr = (int *)mrealloc(NULL, 10 * sizeof(int));
		assert(ptr != NULL);
		mfree(ptr);
	}
	{
		int *ptr = (int *)mmalloc(sizeof(int));
		*ptr = 100;

		ptr = (int *)mrealloc(ptr, 200 * sizeof(int));
		*(ptr + 1) = 200;

		assert(*ptr == 100);
		assert(*(ptr + 1) == 200);

		mfree(ptr);
	}
	{
		int *ptr = (int *)mmalloc(20 * sizeof(int));
		*ptr = 100;
		*(ptr + 1) = 200;

		ptr = (int *)mrealloc(ptr, sizeof(int));
		*ptr = 300;

		assert(*ptr == 300);

		mfree(ptr);
	}
}
