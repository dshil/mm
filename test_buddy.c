#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "buddy.h"

int main()
{
	{
		unsigned char *p = (unsigned char *)mmalloc(1);
		assert(p);
		*p = 255;
		assert(*p == 255);
	}
	{
		char *p = (char *)mmalloc(1);
		char *q = (char *)mmalloc(1);
		assert(p);
		assert(q);
	}
	{
	{
		char *p = (char *)mmalloc(BUFSIZ);
		assert(p);
		*p = 120;
		mfree(p);
	}
	}
	{
		char *p = (char *)mmalloc(1);
		char *q = (char *)mmalloc(1);
		assert(p);
		assert(q);
		*p = *q = 120;
		mfree(p);
		mfree(q);
	}
	{
		assert(mmalloc(53));
		assert(mmalloc(32));
		assert(!mmalloc(0));
	}
}
