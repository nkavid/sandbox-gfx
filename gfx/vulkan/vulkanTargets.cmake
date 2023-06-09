add_library(vulkan_context STATIC)

target_sources(vulkan_context PRIVATE ${CMAKE_CURRENT_LIST_DIR}/context.cpp)

target_include_directories(vulkan_context PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../)

target_link_libraries(
  vulkan_context
  glfw
  vulkan
  utils::logger
)

add_library(vulkan_shader STATIC)

target_sources(vulkan_shader PRIVATE ${CMAKE_CURRENT_LIST_DIR}/shader.cpp)

target_link_libraries(vulkan_shader vulkan)

add_library(vulkan_application STATIC)

target_sources(
  vulkan_application PRIVATE ${CMAKE_CURRENT_LIST_DIR}/application.cpp
)

target_include_directories(
  vulkan_application SYSTEM PRIVATE third_party/glm third_party/stb
)

target_link_libraries(
  vulkan_application
  vulkan_context
  vulkan_shader
  vulkan
  $<TARGET_OBJECTS:stb_image_implementation>
)

add_custom_target(
  ${CMAKE_PROJECT_NAME}_generate_spirv_from_glsl ALL
  COMMAND
    glslc -fshader-stage=vertex
    ${CMAKE_CURRENT_LIST_DIR}/shaders/shader.vert.glsl -o
    ${CMAKE_CURRENT_LIST_DIR}/shaders/shader.vert.spv
  COMMAND
    glslc -fshader-stage=fragment
    ${CMAKE_CURRENT_LIST_DIR}/shaders/shader.frag.glsl -o
    ${CMAKE_CURRENT_LIST_DIR}/shaders/shader.frag.spv
)

add_dependencies(
  vulkan_application ${CMAKE_PROJECT_NAME}_generate_spirv_from_glsl
)

add_executable(vulkan-application ${CMAKE_CURRENT_LIST_DIR}/main.cpp)

target_include_directories(
  vulkan-application PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../
)

target_link_libraries(vulkan-application vulkan_application utils::logger)
