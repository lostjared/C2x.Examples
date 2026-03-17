cmake_minimum_required(VERSION 3.20)
project(c2x VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
#set(CMAKE_C_EXTENSIONS OFF)

find_package(Threads REQUIRED)

add_compile_options(
    -Wall 
    -Wextra 
    -Wpedantic
    -Warray-bounds=2 
    -Wvla-parameter
    -Wshadow
    -Wconversion
)

add_compile_options(
    "$<$<CONFIG:Debug>:-fsanitize=address;-fno-omit-frame-pointer;-g>"
    "$<$<CONFIG:Release>:-O3;-march=native;-flto>"
)

add_link_options(
    "$<$<CONFIG:Debug>:-fsanitize=address>"
    "$<$<CONFIG:Release>:-flto>"
)

add_executable(helloworld src/helloworld.c)
add_executable(hash-test src/hash.c)
add_executable(sort-test src/sort.c)
add_executable(sort-ptr-test src/sort_ptr.c)
add_executable(qsort-test src/qsort.c)
add_executable(hello-world src/main.c)
add_executable(seq-test src/seq.c)
add_executable(log-test src/log.c)
add_executable(struct-test src/struct.c)
add_executable(struct-impl src/struct-impl.c)
add_executable(inline-test src/inline/inline.c)
add_executable(maybe-test src/maybe.c)
add_executable(llist-test src/llist.c)
add_executable(llist-sort-test src/llist-sort.c)
add_executable(auto-test src/auto.c)
add_executable(getline-test src/getline/getline.c src/getline/main.c)
add_executable(embed-test src/embed/embed.c)
add_executable(if-embed-test src/embed/if-embed.c)
add_executable(quine-embed-test src/embed/quine.c)
add_executable(nodiscard-test  src/no-discard.c)
add_executable(static-arg-test src/static-arg.c)
add_executable(restrict-test src/restrict.c)
add_executable(repo-test src/repo.c)
add_executable(typewriter src/typewriter.c)
add_executable(tree-test src/tree.c)
add_executable(restrict-static src/restrict-static.c)
add_executable(trace-f src/tracef.c)
add_executable(skip-cp src/skip-cp.c)
if(CMAKE_SYSTEM_PROCESSOR STREQUAL "x86_64" AND LINUX)
add_executable(inline_asm_test src/inline-asm.c)
endif()
add_executable(generic-test1 src/generic-test1.c)
add_executable(static-test1 src/static-test1.c)
add_executable(count src/count.c)
add_executable(count-char src/count-char.c)
add_executable(count-words src/count-words.c)
add_executable(thread-ex1 src/thread-ex1.c)

target_link_libraries(thread-ex1 PRIVATE Threads::Threads)
