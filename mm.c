#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

#include "mm.h"

#pragma clang diagnostic ignored "-Wdeprecated-declarations"

Header base;
Header *freep = NULL;

static void ensure_init_freelist(void);
static inline unsigned long num_of_blocks(unsigned long size);

static Header *mm(unsigned long size);
static void *mm_sbrk(unsigned long size);
static void *mm_mmap(unsigned long size);

/*
 * mmaloc doesn't guarantee for returned memory area to be zeroed. It's
 * possible that returned area will be zeroed because it might be the first
 * call and we just acquired a big chunk of memory from the kernel. Kernel
 * empties returned memory area for security reason. If it won't do it the
 * process isolation idiom can be broken (some process will receive a
 * previously used memory region by another process).
 *
 * Bad usage:
 *   char *p = (char *)mmaloc(sizeof(long) * 256);
 *   // Do some stuff with @p;
 *   free(p);
 *
 *   char *cp = (char *)mmaloc(sizeof(int) * BUFSIZ);
 *   // Do some stuff with @cp
 *   // @cp can contain some unexpected previously used data.
 */
void *mmalloc(const unsigned long size)
{
	ensure_init_freelist();

	const unsigned long alloc_sz = num_of_blocks(size);

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
			if ((p = mm(alloc_sz)) == NULL)
				return NULL;
	}

	return NULL;
}

void *mcalloc(unsigned long count, unsigned long size)
{
	unsigned long num = count * size;
	char *ptr = NULL;

	if ((ptr = mmalloc(num)) == NULL)
		return NULL;

	memset(ptr, 0, num);
	return ptr;
}
/*
 * mrealloc allows to grow and shrink the memory area pointed by @ptr
 */
void *mrealloc(void *ptr, unsigned long size)
{
	if (ptr == NULL)
		return mmalloc(size);

	Header *bp = (Header*)ptr - 1;
	if (bp == NULL)
		return NULL;

	const unsigned long alloc_sz = num_of_blocks(size);

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
	if (p == NULL)
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
	if (ptr == NULL)
		return;

	Header *bp = (Header *)ptr - 1;
	if (bp == NULL)
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
void mfree_arbitrary(void *ptr, unsigned long size)
{
	if (ptr == NULL || size == 0 || size < sizeof(Header))
		return;

	Header *hp = (Header *)ptr;
	hp->h.size = size / sizeof(Header);
	mfree((void *)(hp + 1));
}

static Header *mm(unsigned long size)
{
	if (size < BLOCKSIZ)
		size = BLOCKSIZ;

	Header *p = NULL;
	void *ptr = NULL;

	if (size >= MMAP_THRESHOLD)
		ptr = mm_mmap(size * sizeof(Header));
	else
		ptr = mm_sbrk(size * sizeof(Header));

	if (ptr == NULL)
		return NULL;

	p = (Header *)ptr;
	p->h.size = size;

	mfree((void *)(p + 1));
	return freep;
}

static void *mm_mmap(unsigned long size)
{
	void *p = NULL;

#ifdef __APPLE__
	p = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANON, -1, 0);
#else
	p = mmap(0, size, PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
#endif // ifdef __APPLE__

	return (p == MAP_FAILED) ? NULL : p;
}

static void *mm_sbrk(unsigned long size)
{
	void *p = NULL;
	return ((p = sbrk(size)) == (void *)(-1)) ? NULL : p;
}

static void ensure_init_freelist(void)
{
	if (freep == NULL) {
		base.h.size = 0;
		base.h.ptr = &base;
		freep = base.h.ptr;
	}
}

static inline unsigned long num_of_blocks(unsigned long size)
{
	return (size + sizeof(Header) - 1) / sizeof(Header) + 1;
}
