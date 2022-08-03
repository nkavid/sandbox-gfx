add_library(vocabulary_uri STATIC)

target_sources(vocabulary_uri PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/uri.cpp
)

target_include_directories(vocabulary_uri PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}/../
)

add_library(vocabulary::uri ALIAS vocabulary_uri)
