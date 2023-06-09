add_library(vocabulary_uri STATIC)

target_sources(vocabulary_uri PRIVATE ${CMAKE_CURRENT_LIST_DIR}/uri.cpp)

target_include_directories(vocabulary_uri PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../)

add_library(vocabulary::uri ALIAS vocabulary_uri)

add_library(vocabulary INTERFACE)

target_sources(
  vocabulary
  INTERFACE
    FILE_SET
    HEADERS
    FILES
    ${CMAKE_CURRENT_LIST_DIR}/size.hpp
)

target_include_directories(
  vocabulary INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/gfx>
                       $<INSTALL_INTERFACE:gfx>
)

install(
  TARGETS vocabulary
  EXPORT gfxTargets
  FILE_SET HEADERS
)
