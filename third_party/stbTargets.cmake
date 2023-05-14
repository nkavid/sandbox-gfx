add_library(stb_image_implementation OBJECT
  ${CMAKE_CURRENT_LIST_DIR}/stb_image_implementation.cpp
)

target_include_directories(stb_image_implementation SYSTEM PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/stb
)

ignore_third_party(stb_image_implementation)
