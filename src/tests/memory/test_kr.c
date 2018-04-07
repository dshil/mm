#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "memory/kr.h"
#include "memory/mm.h"
#include "tester.h"

int main()
{
	test_main();

	{
		char pool[BUFSIZ];
		char *ptr = pool;
		mfree_arbitrary((void *)ptr, BUFSIZ);

		int *p = (int *)mmalloc(10 * sizeof(int));
		assert(p != NULL);
		*p = 1000;
		assert(*p == 1000);
	}
}
