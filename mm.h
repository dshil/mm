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

//              Block
//   +---------------+-------------+
//   | Header | Data | Data | Data |
//   +--------+------+-------------+
//   sizeof(Header) = sizeof(Data)
//
void *mmalloc(unsigned size); /* Basic malloc implementation. */

/*
 * Iterate throught a list of free blocks to find a place where to put freed
 * memory. This place can be found between 2 blocks or on the one of the list
 * ends.
 */
void mfree(void *ptr);

Header *mm(unsigned size); /* Ask Kernel of an additional chunk of memory. */

