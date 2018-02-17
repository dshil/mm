#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "mm.h"

int main()
{
	void *ptr = mmalloc(2);
	assert(ptr != NULL);
	mfree(ptr);
}
