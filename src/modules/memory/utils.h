#ifndef MM_UTILS_H_
#define MM_UTILS_H_

#ifndef __WORDSIZE
#define __WORDSIZE (__SIZEOF_LONG__ * 8)
#endif // ifdef __WORDSIZE

#define BITS_PER_LONG __WORDSIZE
#define BITS_PER_HALF_WORD (__WORDSIZE / 2)

/*
 * Returns @size bytes allocated by portable version of mmap(2).
 *
 * Supported Platforms:
 *  - Linux
 *  - OS X Yosimite 10.10.5 and later
 */
void *mm_mmap(size_t size);
int mm_munmap(void *ptr, size_t size);

/*
 * Returns the result of multiplication @count and @size. SIZE_MAX will be
 * returned in case of overflow.
 */
size_t safe_mul(size_t count, size_t size);

unsigned next_pow_of_2(size_t size);
int is_pow_of_2(size_t size);

#endif // MM_UTILS_H_
