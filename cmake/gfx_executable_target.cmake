function(gfx_executable_target)
  set(options)
  set(one_value_args
      TARGET
      MAIN
  )
  set(multi_value_args
      DEPENDENCIES
  )
  cmake_parse_arguments(
    GFX_EXECUTABLE
    "${options}"
    "${one_value_args}"
    "${multi_value_args}"
    ${ARGN}
  )

  add_executable(
    ${GFX_EXECUTABLE_TARGET}
    ${GFX_EXECUTABLE_MAIN}
  )

  target_link_libraries(
    ${GFX_EXECUTABLE_TARGET}
    ${GFX_EXECUTABLE_DEPENDENCIES}
  )

endfunction()
