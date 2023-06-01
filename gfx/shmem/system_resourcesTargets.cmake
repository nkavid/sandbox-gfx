add_library(
  shmem_writer
  STATIC
)

target_sources(
  shmem_writer
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/writer.cpp
)

target_include_directories(
  shmem_writer
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../
)

target_link_libraries(
  shmem_writer
  rt
  pthread
)

add_library(
  shmem_reader
  STATIC
)

target_sources(
  shmem_reader
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/reader.cpp
)

target_include_directories(
  shmem_reader
  PRIVATE ${CMAKE_CURRENT_LIST_DIR}/../
)

target_link_libraries(
  shmem_reader
  rt
  pthread
)

add_library(
  system_resources::shmem_writer
  ALIAS
  shmem_writer
)
add_library(
  system_resources::shmem_reader
  ALIAS
  shmem_reader
)
