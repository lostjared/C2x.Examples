cmake_minimum_required(VERSION 3.20)
project(c2x VERSION 1.0.0 LANGUAGES C)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(hash-test src/hash.c)
add_executable(sort-test src/sort.c)
add_executable(sort-ptr-test src/sort_ptr.c)
add_executable(hello-world src/main.c)
add_executable(seq-test src/seq.c)
add_executable(log-test src/log.c)
add_executable(struct-test src/struct.c)
add_executable(inline-test src/inline/inline.c)
