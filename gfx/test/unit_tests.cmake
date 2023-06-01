add_executable(unit_tests)

target_link_libraries(unit_tests PRIVATE Catch2::Catch2WithMain)

function(obj_unit_test unit)
  set(varargs DEPENDENCIES INCLUDE_PATH)
  cmake_parse_arguments(OBJ_UNIT_TEST "" "" "${varargs}" ${ARGN})

  add_library(obj_${unit}_test OBJECT
              ${CMAKE_CURRENT_LIST_DIR}/${unit}_test.cpp)

  target_include_directories(
    obj_${unit}_test PRIVATE ${CMAKE_SOURCE_DIR}/${OBJ_UNIT_TEST_INCLUDE_PATH})

  target_link_libraries(obj_${unit}_test PRIVATE ${OBJ_UNIT_TEST_DEPENDENCIES})

  set_target_properties(
    obj_${unit}_test
    PROPERTIES CXX_CLANG_TIDY "${GFX_CLANG_TIDY_CATCH2_TARGET_PROPERTIES}"
               CXX_INCLUDE_WHAT_YOU_USE "${GFX_IWYU_CATCH2_TARGET_PROPERTIES}")

  target_link_libraries(unit_tests PRIVATE obj_${unit}_test)
endfunction()

obj_unit_test(color INCLUDE_PATH gfx/vocabulary/)

obj_unit_test(size INCLUDE_PATH gfx/vocabulary/)

obj_unit_test(
  shmem
  DEPENDENCIES
  system_resources::shmem_writer
  system_resources::shmem_reader
  stubs::utils::logger
  INCLUDE_PATH
  gfx/shmem/)

obj_unit_test(uri DEPENDENCIES vocabulary::uri stubs::utils::logger
              INCLUDE_PATH gfx/vocabulary/)

obj_unit_test(arg_parser DEPENDENCIES utils::arg_parser INCLUDE_PATH gfx/utils/
              gfx/)

obj_unit_test(pip_output_parser DEPENDENCIES google::re2 INCLUDE_PATH
              gfx/utils/)

obj_unit_test(argparse DEPENDENCIES utils::c::argparse INCLUDE_PATH
              gfx/utils/argparse)
