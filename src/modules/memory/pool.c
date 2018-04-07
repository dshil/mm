#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "pool.h"

static pool_config_t *confp = NULL;
static char *bufp = NULL;

static char *pop_block_front(void);
static void push_block_front(char *ptr);
static int ensure_pool_init(void);
static int init_freelist(char *begin);

int mm_initialize_pool(pool_config_t *config)
{
	if (confp) {
		if (confp->alloc_type == MMAP_ALLOCATOR) {
			if (mm_munmap(bufp, confp->pool_size) == -1)
				return -1;
		} else if (confp->alloc_type == MALLOC_ALLOCATOR) {
			free(bufp);
		}
	}
	confp = config;
	bufp = config->begin;

	return init_freelist(config->begin);
}

void mm_pool_put(void *ptr)
{
	char *p = (char *)ptr;
	if (!p)
		return;

	if (ensure_pool_init() == -1)
		return;

	if (p < confp->begin || p > confp->begin + confp->pool_size)
		return;

	push_block_front(ptr);
}

void *mm_pool_get(void)
{
	if (ensure_pool_init() == -1)
		return NULL;
	return pop_block_front();
}

static int ensure_pool_init(void)
{
	if (confp)
		return 0;

	static pool_config_t conf = {
		.begin = NULL,
		.pool_size = BUFSIZ * BUFSIZ,
		.elem_size = (BUFSIZ * BUFSIZ) / BUFSIZ,
		.alloc_type = MMAP_ALLOCATOR,
	};
	confp = &conf;

	char *begin = (bufp = mm_mmap(confp->pool_size));
	return init_freelist(begin);
}

static int init_freelist(char *begin)
{
	if (!begin)
		return -1;

	for (size_t n = 0; n < confp->pool_size/confp->elem_size; n++)
		push_block_front(begin + (n * confp->elem_size));

	return (!confp->begin) ? -1 : 0;
}

static void push_block_front(char *ptr)
{
	pool_node_t *head = (pool_node_t *)confp->begin;
	pool_node_t *bp = (pool_node_t *)ptr;

	if (head) {
		bp->next = head;
		bp->prev = NULL;
		head->prev = bp;
	} else {
		bp->prev = bp->next = NULL;
	}
	confp->begin = (char *)bp;
}

static char *pop_block_front(void)
{
	pool_node_t *head = (pool_node_t *)confp->begin;
	if (!head)
		return NULL;

	pool_node_t *bp = head->next;
	head->next = head->prev = NULL;
	if (bp)
		bp->prev = NULL;

	confp->begin = (char *)bp;
	return (char *)head;
}
