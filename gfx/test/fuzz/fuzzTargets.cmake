if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  if(GFX_ASAN AND GFX_UBSAN)

    add_executable(uriFuzzer)

    target_sources(uriFuzzer PRIVATE ${CMAKE_CURRENT_LIST_DIR}/uri.cpp)

    set_target_properties(
      uriFuzzer
      PROPERTIES COMPILE_FLAGS "-fsanitize=fuzzer"
                 LINK_FLAGS "-fsanitize=fuzzer"
    )

    target_include_directories(
      uriFuzzer PRIVATE ${CMAKE_SOURCE_DIR}/gfx/vocabulary
    )

    target_link_libraries(
      uriFuzzer PRIVATE vocabulary::uri stubs::utils::logger
    )

  endif()
endif()
