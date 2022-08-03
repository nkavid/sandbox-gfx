add_library(window STATIC)
target_sources(window PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/window.cpp
)

target_include_directories(window PRIVATE
  gfx
)

target_link_libraries(window
  ${GL_LIB}
  glfw
  GLEW
)

add_library(components STATIC)
target_sources(components PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/quad.cpp
  ${CMAKE_CURRENT_LIST_DIR}/shader.cpp
  ${CMAKE_CURRENT_LIST_DIR}/detail/compile_shader_program.cpp
  ${CMAKE_CURRENT_LIST_DIR}/texture.cpp
  ${CMAKE_CURRENT_LIST_DIR}/utils/graphics_dump.cpp
)

target_include_directories(components SYSTEM PRIVATE
  third_party/stb
)

target_include_directories(components PRIVATE
  gfx
)

target_link_libraries(components
  ${GL_LIB}
  $<TARGET_OBJECTS:stb_image_implementation>
  GLEW
)

add_custom_target(${CMAKE_PROJECT_NAME}_generate_glsl_string ALL
  COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/glsl_file_to_string.py
    --program-name basic
    --shader-stages vertex fragment
    --shaders-path ${CMAKE_SOURCE_DIR}/gfx/shaders/
  COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/glsl_file_to_string.py
    --program-name texture
    --shader-stages vertex fragment
    --shaders-path ${CMAKE_SOURCE_DIR}/gfx/shaders/
  COMMAND python3 ${CMAKE_SOURCE_DIR}/tools/glsl_file_to_string.py
    --program-name yuv
    --shader-stages compute
    --shaders-path ${CMAKE_SOURCE_DIR}/gfx/shaders/
)

add_dependencies(components ${CMAKE_PROJECT_NAME}_generate_glsl_string)

add_library(graphics::window ALIAS window)
add_library(graphics::components ALIAS components)
