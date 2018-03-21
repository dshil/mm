typedef long Align;

union header {
	struct {
		union header *ptr;
		unsigned size;
	} h;
	Align x;
};

typedef union header Header;

#define BLOCKSIZ 4096
#define MMAP_THRESHOLD (BLOCKSIZ*4)

#ifndef __WORDSIZE
#define __WORDSIZE (__SIZEOF_LONG__ * 8)
#endif // ifdef __WORDSIZE

#define BITS_PER_LONG __WORDSIZE
#define BITS_PER_HALF_WORD (__WORDSIZE / 2)

void *mmalloc(size_t size);
void *mcalloc(size_t count, size_t size);
void *mrealloc(void *ptr, size_t size);
void mfree(void *ptr);
void mfree_arbitrary(void *ptr, size_t size);

