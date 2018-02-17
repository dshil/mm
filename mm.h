typedef long Align;

union header {
	struct {
		union header *ptr;
		unsigned size;
	} h;
	Align x;
};

typedef union header Header;

#define BLOCKSIZ 1024

void *mmalloc(unsigned size);
void *mcalloc(unsigned count, unsigned size);
void *mrealloc(void *ptr, unsigned size);
void mfree(void *ptr);


Header *_mm(unsigned size); /* Ask Kernel of an additional chunk of memory. */

