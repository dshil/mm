typedef struct _buddy_node {
    struct _buddy_node *prev;
    struct _buddy_node *next;
} Header;

typedef struct block_meta {
	size_t size;
	size_t index;
	size_t index_on_level;
	unsigned bytes_offset;
	unsigned bits_offset;
} block_meta;

#define BUDDY_MAX_LEVEL 32
