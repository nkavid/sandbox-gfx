add_library(demo_application STATIC)

target_sources(demo_application PRIVATE
  gfx/applications/demo_application.cpp
)

target_include_directories(demo_application PRIVATE
  gfx
)

target_link_libraries(demo_application
  vocabulary::uri
  graphics::window
  graphics::components
  compute::components
  fmt::fmt
  utils::logger
)

target_include_directories(demo_application PRIVATE
  gfx
)

add_executable(demo-application
  gfx/applications/demo_application_main.cpp
)

target_link_libraries(demo-application
  demo_application
)

include(gfx/input/inputTargets.cmake)

add_executable(gamepad-demo
  gfx/applications/gamepad_demo_main.cpp
)

target_include_directories(gamepad-demo PRIVATE
  gfx
)

target_link_libraries(gamepad-demo
  graphics::window
  input::gamepad
  utils::logger
)

add_library(dummy_video_muxer STATIC
  gfx/utils/muxer.cpp
)

target_include_directories(dummy_video_muxer PRIVATE
  gfx
)

target_link_libraries(dummy_video_muxer
  ffmpeg::libavcodec
  ffmpeg::libavformat
  ffmpeg::libswscale
  fmt::fmt
  utils::arg_parser
)

add_executable(muxing gfx/applications/muxing_main.cpp)

target_include_directories(muxing PRIVATE
  gfx
)

target_link_libraries(muxing
  dummy_video_muxer
  utils::logger
  vocabulary::uri
)

add_executable(opengl-compute-shader
  gfx/applications/opengl_compute_shader_main.cpp
)

target_include_directories(opengl-compute-shader PRIVATE
  gfx
)

target_link_libraries(opengl-compute-shader
  graphics::window
  graphics::components
  utils::logger
)

