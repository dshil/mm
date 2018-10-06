#ifndef MM_KR_H_
#define MM_KR_H_

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

void mfree_arbitrary(void *ptr, size_t size);

#endif // MM_KR_H_
