#ifndef MM_IALLOCATOR_H_
#define MM_IALLOCATOR_H_

#include <stddef.h>
#include <type_traits>

namespace core {

class IAllocator {
public:
    virtual ~IAllocator();

    virtual void *alloc(size_t size) = 0;
    virtual void dealloc(void *ptr) = 0;
};

} // namespace core

namespace {

template <typename T>
struct IsPOD {
    static const bool Value = std::is_pod<T>::value;
};

template <bool T>
struct IntToType {
};

typedef IntToType<false> NonPODType;
typedef IntToType<true> PODType;

const size_t preamble_offset = __SIZEOF_LONG__;

template <typename T>
void dealloc(T *ptr, core::IAllocator &allocator) {
    ptr->~T();
    allocator.dealloc(ptr);
}

template <typename T>
T *alloc_array(size_t count, core::IAllocator &allocator, PODType) {
    if (count == 0)
        return NULL;

    const size_t alloc_sz = count * sizeof(T);

    char *ptr = (char *)allocator.alloc(alloc_sz);
    if (!ptr)
        return NULL;

    T *begin = (T *)ptr;
    T *end = begin + count;

    while (begin != end)
        new (begin++) T;

    return begin - count;
}

template <typename T>
T *alloc_array(size_t count, core::IAllocator &allocator, NonPODType) {
    if (count == 0)
        return NULL;

    const size_t alloc_sz = count * sizeof(T) + preamble_offset;

    char *ptr = (char *)allocator.alloc(alloc_sz);
    if (!ptr)
        return NULL;

    *((size_t *)ptr) = count;
    ptr += preamble_offset;

    T *begin = (T *)ptr;
    T *end = begin + count;

    while (begin != end)
        new (begin++) T;

    return begin - count;
}

template<typename T>
void dealloc_array(T *ptr, core::IAllocator &allocator) {
    dealloc_array(ptr, allocator, IntToType<IsPOD<T>::Value>());
}

template <typename T>
void dealloc_array(T *ptr, core::IAllocator &allocator, PODType) {
    if (!ptr)
        return;

    allocator.dealloc(ptr);
}

template <typename T>
void dealloc_array(T *ptr, core::IAllocator &allocator, NonPODType) {
    if (!ptr)
        return;

    char *p = (char *)ptr;
    const size_t count = *((size_t *)(p - preamble_offset));

    for (int n = count - 1; n >= 0; n--)
        (ptr + n)->~T();

    allocator.dealloc(p - preamble_offset);
}

} // namespace

#define MM_ALLOC(type, allocator) new (allocator) type
#define MM_FREE(ptr, allocator) dealloc(ptr, allocator)

#define MM_ALLOC_ARRAY(type, count, allocator) \
    alloc_array<type>(count, allocator, IntToType<IsPOD<type>::Value>())

#define MM_FREE_ARRAY(ptr, allocator) \
    dealloc_array(ptr, allocator)

inline void *operator new(size_t size, core::IAllocator &allocator) throw() {
    return allocator.alloc(size);
}

inline void *operator new[](size_t size, core::IAllocator &allocator) throw() {
    return allocator.alloc(size);
}

template <class T>
inline void operator delete(void *ptr, core::IAllocator &allocator) throw() {
    allocator.dealloc(ptr);
}

template <class T>
inline void operator delete[](void *ptr, core::IAllocator &allocator) throw() {
    allocator.dealloc(ptr);
}

#endif // MM_IALLOCATOR_H_
