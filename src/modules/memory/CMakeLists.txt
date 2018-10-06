project(memory)

if (MM_USE_KR_ALLOCATOR)
    file(GLOB mm_src "utils.c" "pool/*" "kr/*")
else()
    file(GLOB mm_src "utils.c" "pool/*" "buddy/*")
endif (MM_USE_KR_ALLOCATOR)

add_library(mm_memory STATIC ${mm_src})
target_link_libraries(mm_memory m)
