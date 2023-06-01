add_library(
  gamepad
  STATIC
)

target_sources(
  gamepad
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/gamepad.cpp
)

target_link_libraries(
  gamepad
  glfw
)

add_library(
  input::gamepad
  ALIAS
  gamepad
)
