#ifndef __WORDSIZE
#define __WORDSIZE (__SIZEOF_LONG__ * 8)
#endif // ifdef __WORDSIZE

#define BITS_PER_LONG __WORDSIZE
#define BITS_PER_HALF_WORD (__WORDSIZE / 2)

void *mm_sbrk(size_t size);
void *mm_mmap(size_t size);

size_t safe_mul(size_t count, size_t size);

unsigned next_pow_of_2(size_t size);
int is_pow_of_2(size_t size);

