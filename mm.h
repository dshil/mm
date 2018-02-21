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

void *mmalloc(unsigned long size);
void *mcalloc(unsigned long count, unsigned long size);
void *mrealloc(void *ptr, unsigned long size);
void mfree(void *ptr);
void mfree_arbitrary(void *ptr, unsigned long size);

