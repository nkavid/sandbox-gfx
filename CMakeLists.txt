cmake_minimum_required(VERSION 3.25)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(GFX_CMAKE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/cmake)

option(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/gcc_toolchain.cmake)

option(GFX_LINTING "Use iwyu and clang-tidy." OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 23)

set(CMAKE_CUDA_COMPILER /usr/local/cuda-12.1/bin/nvcc)
set(CMAKE_CUDA_ARCHITECTURES 75)
string(APPEND CMAKE_CUDA_FLAGS
  " --generate-line-info"
  " -std=c++20"
)

project(gfx
  DESCRIPTION "Sandbox project with highly collaborative components"
  VERSION 0.0.1
  LANGUAGES C CXX CUDA
)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX
    "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}_${CMAKE_PROJECT_VERSION}"
    CACHE PATH "..." FORCE)
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

find_package(CUDAToolkit 12.0 REQUIRED)
find_package(Catch2 3.2 REQUIRED)

add_subdirectory(third_party/glm EXCLUDE_FROM_ALL SYSTEM)

set(JSON_SystemInclude ON)
add_subdirectory(third_party/json EXCLUDE_FROM_ALL SYSTEM)

find_package(glfw3 3.3 REQUIRED)
find_package(GLEW 2.2 REQUIRED)
find_library(GL_LIB GL REQUIRED)
find_library(X11_LIB X11 REQUIRED)

find_package(PkgConfig 0.29 REQUIRED)
include(cmake/pkgconfig_import_macro.cmake)

gfx_pkgconfig_import(ffmpeg libavcodec 59.37)
gfx_pkgconfig_import(ffmpeg libavformat 59.27)
gfx_pkgconfig_import(ffmpeg libswscale 6.7)

gfx_pkgconfig_import(google re2 10)

find_library(CUVID_LIB nvcuvid REQUIRED)
find_package(fmt 9.0 REQUIRED)
find_package(Boost 1.74 REQUIRED program_options)

include(cmake/clang_tidy.cmake)
include(cmake/iwyu.cmake)

include(cmake/ignore_macro.cmake)
include(third_party/stbTargets.cmake)

include(cmake/gfx_static_library_target.cmake)
include(gfx/vocabulary/vocabularyTargets.cmake)
include(gfx/graphics/graphicsTargets.cmake)
include(gfx/vulkan/vulkanTargets.cmake)
include(gfx/compute/computeTargets.cmake)
include(gfx/shmem/system_resourcesTargets.cmake)
include(gfx/utils/utilsTargets.cmake)

include(gfx/test/unit_tests.cmake)

include(gfx/applications/applicationsTargets.cmake)

install(TARGETS graphics_window
        EXPORT gfxTargets
)

install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/gfx/graphics/window.hpp"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/gfx/graphics"
        COMPONENT gfx
)

export(EXPORT gfxTargets
  FILE "${GFX_CMAKE_OUTPUT_DIRECTORY}/gfxTargets.cmake"
  NAMESPACE gfx::
)

install(EXPORT gfxTargets
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake"
        NAMESPACE gfx::)

write_basic_package_version_file(
  "${GFX_CMAKE_OUTPUT_DIRECTORY}/gfxConfigVersion.cmake"
  COMPATIBILITY ExactVersion
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/gfxConfig.cmake"
  "${GFX_CMAKE_OUTPUT_DIRECTORY}/gfxConfig.cmake"
  INSTALL_DESTINATION
    "${CMAKE_INSTALL_LIBDIR}/cmake/"
)

install(
  FILES
    "${GFX_CMAKE_OUTPUT_DIRECTORY}/gfxConfig.cmake"
    "${GFX_CMAKE_OUTPUT_DIRECTORY}/gfxConfigVersion.cmake"
  DESTINATION
    "${CMAKE_INSTALL_LIBDIR}/cmake"
)

install(CODE "MESSAGE(\"Finished installing ${CMAKE_INSTALL_PREFIX}\")")
