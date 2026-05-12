

add_executable(ex1 main.c)
target_link_libraries(ex1 PRIVATE cmx_gl::cmx_gl)

add_custom_command(TARGET ex1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/frag.spv
        ${CMAKE_CURRENT_SOURCE_DIR}/vert.spv
        $<TARGET_FILE_DIR:ex1>
)

