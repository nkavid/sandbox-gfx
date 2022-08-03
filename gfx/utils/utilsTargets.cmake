add_library(arg_parser STATIC)

target_sources(arg_parser PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/arg_parser.cpp
)

target_link_libraries(arg_parser
  Boost::program_options
)

target_include_directories(arg_parser PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../
)

add_library(utils_logger STATIC)

target_sources(utils_logger PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/logger.cpp
)

target_include_directories(utils_logger PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../
)

add_library(utils_logger_stub STATIC)

set_target_properties(utils_logger_stub PROPERTIES
  CXX_CLANG_TIDY ""
  CXX_INCLUDE_WHAT_YOU_USE ""
)

target_sources(utils_logger_stub PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/logger_stub.cpp
)

target_include_directories(utils_logger_stub PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../
)

add_library(json_parser STATIC)

target_sources(json_parser PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/json_parser.cpp
)

ignore_third_party(json_parser)

target_link_libraries(json_parser
  nlohmann_json::nlohmann_json
)

target_include_directories(json_parser PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../
)


add_executable(app_utils_demuxer ${CMAKE_CURRENT_LIST_DIR}/demuxer.c)

ignore_gfx_target(app_utils_demuxer clang-tidy)

target_link_libraries(app_utils_demuxer
  ffmpeg::libavcodec
  ffmpeg::libavformat
)

add_library(utils::logger ALIAS utils_logger)
add_library(stubs::utils::logger ALIAS utils_logger_stub)
add_library(utils::arg_parser ALIAS arg_parser)
add_library(utils::json_parser ALIAS json_parser)

add_executable(pip-output-parser
  gfx/utils/pip_output_parser_main.cpp
)

target_link_libraries(pip-output-parser
  google::re2
)

install(TARGETS pip-output-parser)

add_library(argparse_c STATIC
  ${CMAKE_CURRENT_LIST_DIR}/argparse/parse.c
  ${CMAKE_CURRENT_LIST_DIR}/argparse/options.c
  ${CMAKE_CURRENT_LIST_DIR}/argparse/print.c
  ${CMAKE_CURRENT_LIST_DIR}/argparse/logging.c
)

add_library(utils::c::argparse ALIAS argparse_c)

target_include_directories(argparse_c PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)

add_executable(argparse-c ${CMAKE_CURRENT_LIST_DIR}/argparse/demo_parse_main.c)

target_link_libraries(argparse-c
  utils::c::argparse
)
