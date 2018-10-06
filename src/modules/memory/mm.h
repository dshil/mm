#ifndef MM_H_
#define MM_H_

/*
 * It doesn't guarantee for returned memory area to be zeroed. It's
 * possible that returned area will be zeroed because it might be the first
 * call and we just acquired a big chunk of memory from the Kernel. The Kernel
 * empties the returned memory area for a security reason. If it won't do it the
 * process isolation idiom can be broken (some process will receive a
 * previously used memory region by another process).
 */
void *mmalloc(size_t size);

/*
 * Allocates enough space for @count objects. Returned memory area will
 * be zeroed.
 *
 * Due to perfomance reason no need to memset(3) just allocated memory because
 * if the user akses a big chunk of memory we know that it'll be fetched from
 * the Kernel and it will empty this area for us due to security reason.
 */
void *mcalloc(size_t count, size_t size);

/*
 * Grows or shrinks the memory area pointed by @ptr.
 */
void *mrealloc(void *ptr, size_t size);

/*
 * Frees a memory pointed by @ptr.
 */
void mfree(void *ptr);

#endif // MM_H_
