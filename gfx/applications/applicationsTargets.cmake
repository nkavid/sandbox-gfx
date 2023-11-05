add_library(demo_application STATIC)

target_sources(demo_application PRIVATE gfx/applications/demo_application.cpp)

target_link_libraries(
  demo_application
  vocabulary::uri
  graphics::window
  graphics::components
  compute::components
  fmt::fmt
  utils::logger
)

append_clang_tidy_check(TARGET demo_application CHECK "gfx-fundamental-type")

gfx_executable_target(
  TARGET demo-application
  MAIN gfx/applications/demo_application_main.cpp
  DEPENDENCIES demo_application
)

append_clang_tidy_check(TARGET demo-application CHECK "gfx-fundamental-type")

add_executable(gamepad-demo gfx/applications/gamepad_demo_main.cpp)

append_clang_tidy_check(TARGET gamepad-demo CHECK "gfx-fundamental-type")

target_link_libraries(
  gamepad-demo
  graphics::window
  input::gamepad
  utils::logger
)

add_library(dummy_video_muxer STATIC gfx/utils/muxer.cpp)

target_link_libraries(
  dummy_video_muxer
  ffmpeg::libavcodec
  ffmpeg::libavformat
  ffmpeg::libswscale
  fmt::fmt
  utils::arg_parser
  vocabulary
)

add_executable(muxing gfx/applications/muxing_main.cpp)

append_clang_tidy_check(TARGET muxing CHECK "gfx-fundamental-type")

target_link_libraries(
  muxing
  dummy_video_muxer
  utils::logger
  vocabulary::uri
)

add_executable(
  opengl-compute-shader gfx/applications/opengl_compute_shader_main.cpp
)

append_clang_tidy_check(
  TARGET opengl-compute-shader CHECK "gfx-fundamental-type"
)

target_link_libraries(
  opengl-compute-shader
  graphics::window
  graphics::components
  utils::logger
)

gfx_executable_target(
  TARGET headless
  MAIN gfx/applications/headless_main.cpp
  DEPENDENCIES
    graphics::window
    compute::components
    utils::logger
    fmt::fmt
)

if(NOT
   CMAKE_CXX_COMPILER_ID
   STREQUAL
   "Clang"
)
  gfx_executable_target(
    TARGET texture-compute
    MAIN gfx/applications/texture_compute_main.cpp
    DEPENDENCIES
      graphics::window
      graphics::components
      compute::components
      utils::arg_parser
      utils::logger
      vocabulary::uri
  )
endif()
