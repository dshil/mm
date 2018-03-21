#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>

#include "kr.h"

#pragma clang diagnostic ignored "-Wdeprecated-declarations"

static Header base;
static Header *freep = NULL;

static void ensure_init_freelist(void);
static inline size_t num_of_blocks(size_t size);
static size_t safe_mul(size_t count, size_t size);
static size_t mul(size_t count, size_t size);
static size_t fast_mul(size_t count, size_t size);

static Header *mm(size_t size);
static void *mm_sbrk(size_t size);
static void *mm_mmap(size_t size);

/*
 * mmalloc doesn't guarantee for returned memory area to be zeroed. It's
 * possible that returned area will be zeroed because it might be the first
 * call and we just acquired a big chunk of memory from the Kernel. The Kernel
 * empties the returned memory area for a security reason. If it won't do it the
 * process isolation idiom can be broken (some process will receive a
 * previously used memory region by another process).
 */
void *mmalloc(const size_t size)
{
	ensure_init_freelist();

	const size_t alloc_sz = num_of_blocks(size);

	Header *prevp = freep;
	Header *p = NULL;

	for (p = prevp->h.ptr; ; prevp = p, p = p->h.ptr) {
		if (p->h.size >= alloc_sz) {
			if (p->h.size == alloc_sz)
				prevp->h.ptr = p->h.ptr;
			else {
				p->h.size -= alloc_sz;
				p += p->h.size;
				p->h.size = alloc_sz;
			}

			freep = prevp;
			return (void *)(p + 1);
		}

		if (p == freep)
			if (!(p = mm(alloc_sz)))
				return NULL;
	}

	return NULL;
}

/*
 * mcalloc allocates enough space for @count objects. Returned memory area will
 * be zeroed.
 *
 * Due to perfomance reason no need to memset(3) just allocated memory because
 * if the user akses a big chunk of memory we know that it'll be fetched from
 * the Kernel and it will empty this area for us due to security reason.
 */
void *mcalloc(size_t count, size_t size)
{
	size_t num = safe_mul(count, size);
	char *ptr = NULL;

	const char first_alloc = (freep == NULL);

	if (!(ptr = mmalloc(num)))
		return NULL;

	Header *bp = (Header *)ptr - 1;
	if (!bp)
		return NULL;

	if (bp->h.size >= BLOCKSIZ && !first_alloc)
		return ptr;

	memset(ptr, 0, num);
	return ptr;
}
/*
 * mrealloc allows to grow and shrink the memory area pointed by @ptr.
 */
void *mrealloc(void *ptr, size_t size)
{
	if (!ptr)
		return mmalloc(size);

	Header *bp = (Header*)ptr - 1;
	if (!bp)
		return NULL;

	const size_t alloc_sz = num_of_blocks(size);

	if (bp->h.size >= alloc_sz) {
		if (bp->h.size == alloc_sz)
			return (void *)(bp + 1);

		Header *p = bp;

		bp->h.size -= alloc_sz;
		p += bp->h.size;
		p->h.size = alloc_sz;

		mfree((void *)(bp + 1));
		return (void *)(p + 1);
	}

	void *p = mmalloc(alloc_sz);
	if (!p)
		return NULL;

	memcpy(p, ptr, (bp->h.size - 1) * sizeof(Header));
	mfree((void *)(bp + 1));

	return p;
}

/*
 * mfree puts a memory area pointed by @ptr to a list of free blocks.
 * Neighbors blocks will be merged.
 */
void mfree(void *ptr)
{
	if (!ptr)
		return;

	Header *bp = (Header *)ptr - 1;
	if (!bp)
		return;

	ensure_init_freelist();

	Header *p = NULL;
	for (p = freep; !(bp > p && bp < p->h.ptr); p = p->h.ptr)
		if (p >= p->h.ptr && (bp > p || bp < p->h.ptr))
				break;

	if (bp + bp->h.size == p->h.ptr) {
		bp->h.size += p->h.ptr->h.size;
		bp->h.ptr = p->h.ptr->h.ptr;
	} else
		bp->h.ptr = p->h.ptr;

	if (p + p->h.size == bp) {
		p->h.size += bp->h.size;
		p->h.ptr = bp->h.ptr;
	} else
		p->h.ptr = bp;

	freep = p;
}

/*
 * mfree_arbitrary puts a memory area in a list of free blocks.
 */
void mfree_arbitrary(void *ptr, size_t size)
{
	if (!ptr || size == 0 || size < sizeof(Header))
		return;

	Header *hp = (Header *)ptr;
	hp->h.size = size / sizeof(Header);
	mfree((void *)(hp + 1));
}

static Header *mm(size_t size)
{
	if (size < BLOCKSIZ)
		size = BLOCKSIZ;

	Header *p = NULL;
	void *ptr = NULL;

	if (size >= MMAP_THRESHOLD)
		ptr = mm_mmap(size * sizeof(Header));
	else
		ptr = mm_sbrk(size * sizeof(Header));

	if (!ptr)
		return NULL;

	p = (Header *)ptr;
	p->h.size = size;

	mfree((void *)(p + 1));
	return freep;
}

static void *mm_mmap(size_t size)
{
	void *p = NULL;

#ifdef __APPLE__
	p = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1, 0);
#else
	p = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
#endif // ifdef __APPLE__

	return (p == MAP_FAILED) ? NULL : p;
}

static void *mm_sbrk(size_t size)
{
	void *p = NULL;
	return ((p = sbrk(size)) == (void *)(-1)) ? NULL : p;
}

static void ensure_init_freelist(void)
{
	if (!freep) {
		base.h.size = 0;
		base.h.ptr = &base;
		freep = base.h.ptr;
	}
}

/*
 * num_of_blocks transforms @size bytes into number of blocks in the free list.
 * overflow-safe.
 */
static inline size_t num_of_blocks(size_t size)
{
	size_t num = size + sizeof(Header);
	return (num <= size || num <= sizeof(Header)) ?
		SIZE_MAX : num - 1 / sizeof(Header) + 1;
}

static size_t safe_mul(size_t count, size_t size)
{
	size_t ret = 0;

#ifdef __SIZEOF_INT128__
	ret = fast_mul(count, size);
#else
	ret = mul(count, size);
#endif // ifdef __SIZEOF_INT128__

	return ret;
}

static size_t mul(size_t count, size_t size)
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

static size_t fast_mul(size_t count, size_t size)
{
	__uint128_t c = count;
	__uint128_t s = size;
	__uint128_t ret = c * s;

	return ret >> BITS_PER_LONG ? SIZE_MAX : ret;
}
