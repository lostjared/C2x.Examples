cmake_minimum_required(VERSION 3.20)
project(cmx_gl VERSION 1.0.0 LANGUAGES C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(SDL3 REQUIRED)
find_package(OpenGL REQUIRED)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
add_compile_options(
	-Wall 
	-Wextra
	-O3
)
else()
add_compile_options(
	-Wall
	-Wextra
    -g
)
endif()

include(GNUInstallDirs)
add_library(cmx_gl SHARED cmx_gl/src/cmx_gl.c cmx_gl/src/glad.c)
target_include_directories(cmx_gl
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/cmx_gl/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/cmx_gl>
)

target_link_libraries(cmx_gl PUBLIC SDL3::SDL3 OpenGL::OpenGL Threads::Threads)
add_library(cmx_gl::cmx_gl ALIAS cmx_gl)
include(CMakePackageConfigHelpers)

option(EXAMPLES "Examples" OFF)

if(EXAMPLES)
	add_subdirectory(ex1)
	add_subdirectory(ex2_compute)
endif()

write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/cmx_glConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmx_glConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmx_glConfig.cmake"
    @ONLY
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/cmx_gl/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cmx_gl
    FILES_MATCHING PATTERN "*.h"
)

install(TARGETS cmx_gl
    EXPORT cmx_gl_Targets
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/cmx_glConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/cmx_glConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/cmx_gl)
