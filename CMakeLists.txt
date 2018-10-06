cmake_minimum_required(VERSION 3.0)
project(mm)

# Setup project options
option(MM_USE_KR_ALLOCATOR "Use Kernighan and Ritchie allocator" OFF)

# Setup project outputs
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin")
set(LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib")

# Setup project search path
include_directories("${PROJECT_SOURCE_DIR}/src/modules")
include_directories("${PROJECT_SOURCE_DIR}/3rdparty/deps")

# Enable a lot of warnings
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} \
    -Wall -Wpedantic -Wextra -Wno-missing-field-initializers")

add_subdirectory(src/modules/memory)
add_subdirectory(src/tests)
add_subdirectory(src/examples)
