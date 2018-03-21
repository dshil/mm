#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "kr.h"

int main()
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
		char pool[BUFSIZ];
		char *ptr = pool;
		mfree_arbitrary((void *)ptr, BUFSIZ);

		int *p = (int *)mmalloc(10 * sizeof(int));
		assert(p != NULL);
		*p = 1000;
		assert(*p == 1000);
	}
	{
		int *ptr = (int *)mrealloc(NULL, 10 * sizeof(int));
		assert(ptr != NULL);
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
