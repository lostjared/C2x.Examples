add_executable(ex2_compute main.c)
target_link_libraries(ex2_compute PRIVATE cmx_gl::cmx_gl)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/vert.spv
    COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/vertex.vert -o ${CMAKE_CURRENT_SOURCE_DIR}/vert.spv
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/vertex.vert
    COMMENT "Compiling vertex.vert to SPIR-V"
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/frag.spv
    COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/fragment.frag -o ${CMAKE_CURRENT_SOURCE_DIR}/frag.spv
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/fragment.frag
    COMMENT "Compiling fragment.frag to SPIR-V"
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/compute.spv
    COMMAND glslc ${CMAKE_CURRENT_SOURCE_DIR}/compute.comp -o ${CMAKE_CURRENT_SOURCE_DIR}/compute.spv
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/compute.comp
    COMMENT "Compiling compute.comp to SPIR-V"
)

add_custom_target(ex2_compute_shaders ALL
    DEPENDS 
        ${CMAKE_CURRENT_SOURCE_DIR}/vert.spv
        ${CMAKE_CURRENT_SOURCE_DIR}/frag.spv
        ${CMAKE_CURRENT_SOURCE_DIR}/compute.spv
)

add_dependencies(ex2_compute ex2_compute_shaders)

add_custom_command(TARGET ex2_compute POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/frag.spv
        ${CMAKE_CURRENT_SOURCE_DIR}/vert.spv
        ${CMAKE_CURRENT_SOURCE_DIR}/compute.spv
        $<TARGET_FILE_DIR:ex2_compute>
)

