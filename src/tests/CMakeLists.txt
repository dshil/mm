project(tests)

add_library(mm_tests STATIC test_main.cpp)

if (MM_USE_KR_ALLOCATOR)
    file(GLOB tests_src "memory/test_main.cpp" "memory/pool/*" "memory/kr/*")
else()
    file(GLOB tests_src "memory/test_main.cpp" "memory/pool/*" "memory/buddy/*")
endif(MM_USE_KR_ALLOCATOR)

add_executable(test_main ${tests_src})
target_link_libraries(test_main PRIVATE mm_memory mm_tests)
