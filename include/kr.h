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

void mfree_arbitrary(void *ptr, size_t size);
