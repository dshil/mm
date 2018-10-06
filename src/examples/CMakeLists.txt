project(examples)

file(GLOB cpp_src "cpp/*")
add_executable(examples ${cpp_src})
target_link_libraries(examples PRIVATE mm_memory mm_tests)
