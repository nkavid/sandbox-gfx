cmake_minimum_required(VERSION 3.25)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(GFX_CMAKE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib/cmake)

set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/cmake/gcc_toolchain.cmake)

option(GFX_IWYU "Enable include-what-you-use." OFF)
option(GFX_CLANG_TIDY "Enable clang-tidy." OFF)
option(GFX_ASAN "Enable address sanitized." OFF)
option(GFX_UBSAN "Enable undefined behavior sanitizer." OFF)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 23)

set(CMAKE_CUDA_ARCHITECTURES 75)

set(CUDA_SEPARABLE_COMPILATION ON)
set(CUDA_PROPAGATE_HOST_FLAGS OFF)

project(
  gfx
  DESCRIPTION "Sandbox project with highly collaborative components"
  VERSION 0.0.1
  LANGUAGES C CXX CUDA
)

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX
      "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}_${CMAKE_PROJECT_VERSION}"
      CACHE PATH "..." FORCE
  )
endif()

include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY true)

include(cmake/ignore_target.cmake)

find_package(CUDAToolkit 12.0 REQUIRED)
find_package(Catch2 3.2 REQUIRED)

add_subdirectory(third_party/glm EXCLUDE_FROM_ALL SYSTEM)

set(JSON_SystemInclude ON)
add_subdirectory(third_party/json EXCLUDE_FROM_ALL SYSTEM)

add_subdirectory(third_party/glfw EXCLUDE_FROM_ALL SYSTEM)
ignore_third_party(glfw)

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
find_package(
  Boost
  1.74
  REQUIRED
  program_options
)

include(cmake/clang_tidy.cmake)
include(cmake/iwyu.cmake)

include(third_party/stbTargets.cmake)
include(third_party/nvidia_video_codec_sdkTargets.cmake)

include(cmake/gfx_static_library_target.cmake)
include(cmake/gfx_executable_target.cmake)

include(gfx/vocabulary/vocabularyTargets.cmake)
include(gfx/graphics/graphicsTargets.cmake)
include(gfx/vulkan/vulkanTargets.cmake)
include(gfx/compute/computeTargets.cmake)
include(gfx/shmem/system_resourcesTargets.cmake)
include(gfx/utils/utilsTargets.cmake)
include(gfx/input/inputTargets.cmake)

include(gfx/test/unit_tests.cmake)
include(gfx/test/fuzz/fuzzTargets.cmake)

include(gfx/applications/applicationsTargets.cmake)

gfx_print_asan_info()
gfx_print_ubsan_info()

export(
  EXPORT gfxTargets
  FILE "${GFX_CMAKE_OUTPUT_DIRECTORY}/gfxTargets.cmake"
  NAMESPACE gfx::
)

install(
  EXPORT gfxTargets
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake"
  NAMESPACE gfx::
)

write_basic_package_version_file(
  "${GFX_CMAKE_OUTPUT_DIRECTORY}/gfxConfigVersion.cmake"
  COMPATIBILITY ExactVersion
)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/cmake/gfxConfig.cmake"
  "${GFX_CMAKE_OUTPUT_DIRECTORY}/gfxConfig.cmake"
  INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/"
)

install(
  FILES "${GFX_CMAKE_OUTPUT_DIRECTORY}/gfxConfig.cmake"
        "${GFX_CMAKE_OUTPUT_DIRECTORY}/gfxConfigVersion.cmake"
  DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake"
)

install(CODE "MESSAGE(\"Finished installing ${CMAKE_INSTALL_PREFIX}\")")
