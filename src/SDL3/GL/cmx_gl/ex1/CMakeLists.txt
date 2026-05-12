add_executable(ex1 main.c)
target_link_libraries(ex1 PRIVATE cmx_gl::cmx_gl)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/vert.spv
    COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/vertex.vert -o ${CMAKE_CURRENT_SOURCE_DIR}/vert.spv
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/vertex.vert
    COMMENT "Compiling vertex.vert to SPIR-V"
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/frag.spv
    COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/frag.frag -o ${CMAKE_CURRENT_SOURCE_DIR}/frag.spv
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/frag.frag
    COMMENT "Compiling frag.frag to SPIR-V"
)

add_custom_target(ex1_shaders ALL
    DEPENDS 
        ${CMAKE_CURRENT_SOURCE_DIR}/vert.spv
        ${CMAKE_CURRENT_SOURCE_DIR}/frag.spv
)

add_dependencies(ex1 ex1_shaders)

add_custom_command(TARGET ex1 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/frag.spv
        ${CMAKE_CURRENT_SOURCE_DIR}/vert.spv
        $<TARGET_FILE_DIR:ex1>
)
