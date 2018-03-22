#include <math.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "mm.h"
#include "buddy.h"
#include "utils.h"

/*
 * Returns a free block for the provided level. If there is no required block
 * on the corresponding level it takes a block from the upper level, splits it
 * into two parts, puts one into the free list and returns another to the
 * caller. If there is no block even for level 0 NULL should be returned
 * signalizing that we've run out of memory.
 *
 * Free blocks are stored in an implicit linked list on each level. Pointers to
 * next and prev element of the list are stored directly in the free blocks.
 * It allows free lists consumes zero extra memory. When the block is extracted
 * from the free list the meta data stored in it will be cleared and the free
 * list pointers will be updated.
 */
static void *alloc_block(unsigned level);

/*
 * Free block on the provided level.
 *
 * If the buddy of the provided block is also free we'll remove the buddy from
 * the freelist on the corresponding level, merge it with the freed block and
 * put the merged block in the freelist of the upper level.
 */
static void free_block(void *block, unsigned level);

static inline size_t total_size(void);
static inline unsigned get_level(size_t bytes);
static inline size_t sizeof_block(unsigned level);
static inline size_t max_blocks_on_level(unsigned level);

static void *pop_block_front(unsigned level);
static void push_block_front(const void *bp, unsigned level);
static void remove_block(const void *bp, unsigned level);

/*
 * returns the total size of meta data.
 *
 * To calculate the meta size explore an example with BUDDY_MAX_LEVEL = 2.
 *
 * +-----------------------+-----------+
 * |           0           |  64 bytes |
 * +-----------------------+-----------+
 * |     1     |     2     |  32 bytes |
 * +-----------+-----------+-----------+
 * |  3  |  4  |  5  |  6  |  16 bytes |
 * +-----+-----+-----+-----+-----------+
 *
 * Number of blocks = 7. The total size = 4 * 16 = 64 bytes.
 *
 * 3 bits is required to determine the block level. The level of the blocks of
 * the last level won't be stored because we assume that if the block doesn't
 * belong to any levels, it belongs to the last one.
 *
 * 3 bits is required to determine if the pair of blocks is free or not. Block
 * on the first level doesn't have a buddy.
 *
 * As a result 6 bits are required for meta data. It'll rounded to 8 bits.
 * We are thinking in terms of bytes and 8 bits will be transformed to 1 byte.
 *
 * The last step is to calculate the number of leafs, the smallest block of the
 * allocator.
 *
 * The leaf size is determined by the buddy_node_t. It should be at least 16
 * bytes to be able to store 2 pointers of free list but keep in mind that this
 * structure is implementation dependent.
 *
 * Returned bytes number will be at least sizeof(buddy_node_t) because for
 * allocator with the small number of levels we'll need less than a byte to
 * store all the meta data. Returned value can be treated as: first half of
 * bytes is used for level index, the second half - for the merge index.
 */
static size_t sizeof_meta(void);

/*
 * In-memory index initialization routine. The whole index is split into 2
 * separate index:
 *  - Level index: used for checking if the level contains this block.
 *  - Merge index: used for checking if the pair of blocks is free or not.
 *
 * The level bit is stored for all blocks except the last one. If the block
 * belongs to the level the corresponding bit is set to 1. As a result the total
 * size of the level index equals to 1 << BUDDY_MAX_LEVEL.
 *
 * The merge bit is used for each pair of blocks. If one of the block of the
 * pair was freed or allocated the corresponding bits is XORed. As a result the
 * total size of the merge index equals to 1 << BUDDY_MAX_LEVEL.
 *
 * For example, if BUDDY_MAX_LEVEL = 2, then the total number of blocks equals
 * to 1 << (BUDDY_MAX_LEVEL + 1) = 7 blocks.
 * Bits required for level index: 1 << BUDDY_MAX_LEVEL = 4 bits.
 * Bits required for the merge index: 1 << BUDDY_MAX_LEVEL = 4 bits.
 * The total size: 8 bits or 1 byte.
 * The total overhead of memory consumption for each block: ~ 1 bit.
 *
 * Meta data is stored right before the free memory.
 */
static void *ensure_meta_init(void);

static inline char *get_level_index(void);
static inline char *get_merge_index(void);

/*
 * Returns the level of the @block.
 * In worst case we need at least (BUDDY_MAX_LEVEL - 1) iteration to determine
 * the block level.
 *
 * As well known when the user allocates the block via malloc(2) he actually
 * knows the size of allocated memory and can provide this size to the free(2).
 * It this user so kind the usage of this routine can be reduced
 * because we can directly determine the level from the block size.
 */
static unsigned find_block_level(const void *block);

static void set_block_level(const void *block, unsigned level);
static int check_block_level(const void *block, unsigned level);
static int toggle_allocated_block(const void *block, unsigned level);
static void fill_buddy_meta(const void *block, unsigned level, buddy_meta_t *bm);

static void *freelists[BUDDY_MAX_LEVEL+1];
static void *bufp = NULL;
static void *metap = NULL;

void *mmalloc(size_t size)
{
	if (size == 0)
		return NULL;

	unsigned level = get_level(size);
	if (level > BUDDY_MAX_LEVEL)
		return NULL;

	if (!ensure_meta_init())
		return NULL;

	level = BUDDY_MAX_LEVEL - level;

	void *ret = alloc_block(level);
	if (!ret)
		return NULL;

	if (level != BUDDY_MAX_LEVEL)
		set_block_level(ret, level);

	return ret;
}

void mfree(void *ptr)
{
	if (!bufp || !metap || !ptr || (ptr < bufp))
		return;
	free_block(ptr, find_block_level(ptr));
}

void *mcalloc(size_t count, size_t size)
{
	const size_t num = safe_mul(count, size);
	if (num == SIZE_MAX)
		return NULL;

	const char first_alloc = (bufp == NULL);
	void *ptr = NULL;

	if (!(ptr = mmalloc(num)))
		return NULL;

	if (first_alloc)
		return ptr;

	memset(ptr, 0, num);

	return ptr;
}

void *mrealloc(void *ptr, size_t size)
{
	if (!ptr)
		return mmalloc(size);

	unsigned realloc_level = get_level(size);
	if (realloc_level > BUDDY_MAX_LEVEL)
		return NULL;

	realloc_level = BUDDY_MAX_LEVEL - realloc_level;

	const size_t realloc_sz = sizeof_block(realloc_level);
	const unsigned block_level = find_block_level(ptr);
	const size_t block_sz = sizeof_block(block_level);

	if (realloc_sz == block_sz)
		return ptr;

	void *p = mmalloc(size);
	if (!p)
		return NULL;

	memcpy(p, ptr, block_sz);
	mfree(ptr);

	return p;
}

void mfree_bytes(void *ptr, size_t size)
{
	if (!bufp || !metap || !ptr || (ptr < bufp))
		return;

	unsigned level = get_level(size);
	if (level > BUDDY_MAX_LEVEL)
		return;

	free_block(ptr, BUDDY_MAX_LEVEL - level);
}

static void free_block(void *block, unsigned level)
{
	const size_t size = sizeof_block(level);
	const size_t index_on_level = (block - bufp) / size;
	void *buddy = (index_on_level % 2 == 0) ? block + size : block - size;

	const int merge = !toggle_allocated_block(block, level);
	if (!merge || level == 0) {
		push_block_front(block, level);
		return;
	}

	remove_block(buddy, level);

	void *fb = (block > buddy) ? buddy : block;
	free_block(fb, level - 1);
}

static void *alloc_block(unsigned level)
{
	void *bp = freelists[level];
	if (!bp) {
		if (level == 0)
			return NULL;

		bp = alloc_block(level - 1);
		if (!bp)
			return NULL;

		push_block_front(bp + sizeof_block(level), level);
		push_block_front(bp, level);
	}

	void *ret = pop_block_front(level);
	if (!ret)
		return NULL;

	toggle_allocated_block(ret, level);
	return ret;
}

static void push_block_front(const void *ptr, unsigned level)
{
	buddy_node_t *head = (buddy_node_t *)freelists[level];
	buddy_node_t *bp = (buddy_node_t *)ptr;

	if (head) {
		bp->next = head;
		bp->prev = NULL;
		head->prev = bp;
	} else {
		bp->prev = bp->next = NULL;
	}

	freelists[level] = (void *)bp;
}

static void *pop_block_front(unsigned level)
{
	buddy_node_t *head = (buddy_node_t *)freelists[level];
	if (!head)
		return NULL;

	buddy_node_t *bp = head->next;
	head->next = head->prev = NULL;
	if (bp)
		bp->prev = NULL;

	freelists[level] = (void *)bp;
	return (void *)head;
}

static void remove_block(const void *block, unsigned level)
{
	buddy_node_t *head = (buddy_node_t *)freelists[level];

	if (head == block) {
		pop_block_front(level);
		return;
	}

	buddy_node_t *p = head;
	for (; p != NULL; p = p->next) {
		if (p == block) {
			p->prev->next = p->next;
			if (p->next)
				p->next->prev = p->prev;
			p->prev = p->next = NULL;
			return;
		}
	}
}

static inline unsigned get_level(size_t bytes)
{
	return (unsigned)log2(next_pow_of_2(bytes) / sizeof(buddy_node_t));
}

static inline size_t total_size(void)
{
	return (1UL << BUDDY_MAX_LEVEL) * sizeof(buddy_node_t);
}

static inline size_t sizeof_block(unsigned level)
{
	return total_size() / max_blocks_on_level(level);
}

static size_t sizeof_meta(void)
{
	const size_t num_blocks = 1UL << (1 + BUDDY_MAX_LEVEL);
	const size_t num_bytes = num_blocks / 8;
	const size_t leafs_num = num_bytes / sizeof(buddy_node_t);

	return (leafs_num == 0) ?
		sizeof(buddy_node_t) : leafs_num * sizeof(buddy_node_t);
}

static inline char *get_merge_index(void)
{
	static size_t offset = 0;
	if (!offset)
		offset = sizeof_meta() / 2;

	return (char *)(metap + offset);
}

static inline char *get_level_index(void)
{
	return (char *)(metap);
}

static inline size_t max_blocks_on_level(unsigned level)
{
	return (1UL << level);
}

static void *ensure_meta_init(void)
{
	if (metap)
		return metap;

	const size_t meta_sz = sizeof_meta();
	if (!(metap = mm_mmap(meta_sz + total_size())))
		return NULL;

	bufp = metap + meta_sz;
	push_block_front(bufp, 0);

	return metap;
}

static unsigned find_block_level(const void *block)
{
	for (int i = BUDDY_MAX_LEVEL - 1; i > 0; i--)
		if (check_block_level(block, i))
			return i;
	return BUDDY_MAX_LEVEL;
}

static void set_block_level(const void *block, unsigned level)
{
	buddy_meta_t bm;
	fill_buddy_meta(block, level, &bm);

	char *level_index = get_level_index() + bm.bytes_offset;
	*level_index |= (1UL << bm.bits_offset);
}

static int check_block_level(const void *block, unsigned level)
{
	buddy_meta_t bm;
	fill_buddy_meta(block, level, &bm);

	char *level_index = get_level_index() + bm.bytes_offset;
	return (*level_index >> bm.bits_offset) & 1UL;
}

static int toggle_allocated_block(const void *block, unsigned level)
{
	buddy_meta_t bm;
	fill_buddy_meta(block, level, &bm);

	if (bm.index_on_level % 2 != 0)
		fill_buddy_meta(block - bm.size, level, &bm);

	char *merge_index = get_merge_index() + bm.bytes_offset;
	*merge_index ^= (1UL << bm.bits_offset);

	return (*merge_index >> bm.bits_offset) & 1U;
}

static void fill_buddy_meta(const void *block, unsigned level, buddy_meta_t *bm)
{
	memset(bm, 0, sizeof(buddy_meta_t));
	bm->size = sizeof_block(level);
	bm->index_on_level = (block - bufp) / bm->size;
	bm->index = (1UL << level) - 1 + bm->index_on_level;
	bm->bytes_offset = bm->index / 8;
	bm->bits_offset = bm->index % 8;
}
