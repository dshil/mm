#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "include/mm.h"
#include "include/buddy.h"
#include "include/tester.h"

int main()
{
	test_main();
	{
		const size_t size = 100;
		char *p = (char *)mmalloc(size);
		assert(p);
		mfree_bytes(p, size);
	}
}
