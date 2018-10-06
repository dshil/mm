#ifndef MM_POOL_H_
#define MM_POOL_H_

/*
 * Pool is used to dynamically allocate/free blocks of fixed size from the fixed
 * pool of memory.
 */

/*
 * Element of the implicit linked list of free blocks.
 */
typedef struct _pool_node {
    struct _pool_node *prev;
    struct _pool_node *next;
} pool_node_t;

typedef enum _pool_allocator {
    MMAP_ALLOCATOR   = 1,
    MALLOC_ALLOCATOR = 2,
} pool_allocator_t;

typedef struct pool_config {
    char *begin;
    size_t pool_size;
    size_t elem_size;
    pool_allocator_t alloc_type;
} pool_config_t;

void mm_pool_put(void *ptr);
void *mm_pool_get(void);
int mm_initialize_pool(pool_config_t *config);

#endif // MM_POOL_H_
