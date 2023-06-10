add_library(compute_components STATIC)

target_sources(
  compute_components
  PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}/context.cpp
    ${CMAKE_CURRENT_LIST_DIR}/pixel_buffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/texture_buffer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/utils/compute_dump.cpp
    ${CMAKE_CURRENT_LIST_DIR}/circle.cu
)

if(NOT
   CMAKE_CXX_COMPILER_ID
   STREQUAL
   "Clang"
)
  target_sources(
    compute_components PRIVATE ${CMAKE_CURRENT_LIST_DIR}/circle_texture.cu
  )
endif()

target_include_directories(compute_components SYSTEM PRIVATE third_party/stb)

target_include_directories(compute_components PRIVATE gfx)

target_link_libraries(
  compute_components
  CUDA::cuda_driver
  $<TARGET_OBJECTS:stb_image_implementation>
)

add_library(compute::components ALIAS compute_components)
