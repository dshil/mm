#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "include/kr.h"
#include "include/mm.h"
#include "include/utils.h"

static Header base;
static Header *freep = NULL;

static void ensure_init_freelist(void);
static inline size_t num_of_blocks(size_t size);
static Header *mm(size_t size);

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

void *mcalloc(size_t count, size_t size)
{
	size_t num = safe_mul(count, size);
	if (num == SIZE_MAX)
		return NULL;

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

	ptr = mm_mmap(size * sizeof(Header));

	if (!ptr)
		return NULL;

	p = (Header *)ptr;
	p->h.size = size;

	mfree((void *)(p + 1));
	return freep;
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
