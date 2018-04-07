#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "memory/mm.h"
#include "memory/buddy.h"
#include "tester.h"

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
