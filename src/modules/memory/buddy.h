/*
 * Element of the implicit linked list of free blocks.
 */
typedef struct _buddy_node {
    struct _buddy_node *prev;
    struct _buddy_node *next;
} buddy_node_t;

typedef struct _buddy_meta {
	size_t size;
	size_t index;
	size_t index_on_level;
	unsigned bytes_offset;
	unsigned bits_offset;
} buddy_meta_t;

#define BUDDY_MAX_LEVEL 28

/*
 * Frees block the same as mfree does but makes it faster because we don't spend
 * time finding level of the freed block.
 */
void mfree_bytes(void *ptr, size_t size);
