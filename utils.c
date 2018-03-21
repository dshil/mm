#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/mman.h>

#include "utils.h"

#pragma clang diagnostic ignored "-Wdeprecated-declarations"

void *mm_mmap(size_t size)
{
	void *p = NULL;

#ifdef __APPLE__
	p = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1, 0);
#else
	p = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
#endif // ifdef __APPLE__

	return (p == MAP_FAILED) ? NULL : p;
}

void *mm_sbrk(size_t size)
{
	void *p = NULL;
	return ((p = sbrk(size)) == (void *)(-1)) ? NULL : p;
}

size_t safe_mul(size_t count, size_t size)
{
	size_t ret = 0;

#ifdef __SIZEOF_INT128__
	ret = fast_mul(count, size);
#else
	ret = mul(count, size);
#endif // ifdef __SIZEOF_INT128__

	return ret;
}

size_t mul(size_t count, size_t size)
{
	unsigned count_high = count >> BITS_PER_HALF_WORD;
	unsigned count_low = count;

	unsigned size_high = size >> BITS_PER_HALF_WORD;
	unsigned size_low = size;

	if (!count_high && !size_high)
		return count * size;

	if (count_high && size_high)
		return SIZE_MAX;

	size_t t1 = 0;
	size_t t2 = 0;

	if (count_high) {
		t1 = (size_t)count_high * size_low;
		t2 = (size_t)count_low * size_low;
	} else {
		t1 = (size_t)size_high * count_low;
		t2 = (size_t)size_low * count_low;
	}

	if ((t1 + (t2 >> BITS_PER_HALF_WORD)) >= UINT32_MAX)
		return SIZE_MAX;

	return (t1 << BITS_PER_HALF_WORD) + t2;
}

size_t fast_mul(size_t count, size_t size)
{
	__uint128_t c = count;
	__uint128_t s = size;
	__uint128_t ret = c * s;

	return ret >> BITS_PER_LONG ? SIZE_MAX : ret;
}

int is_pow_of_2(size_t size)
{
	return !(size & (size - 1));
}

unsigned next_pow_of_2(size_t size)
{
	if (is_pow_of_2(size))
		return size;

	size = size - 1;
	size = size | (size >> 1);
	size = size | (size >> 2);
	size = size | (size >> 4);
	size = size | (size >> 8);
	size = size | (size >>16);

	return size + 1;
}
