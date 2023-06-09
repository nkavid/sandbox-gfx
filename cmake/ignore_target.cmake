# Skip checking third party targets
function(ignore_third_party target_name)
  set_target_properties(
    ${target_name}
    PROPERTIES
      CXX_CLANG_TIDY ""
      CXX_INCLUDE_WHAT_YOU_USE ""
      C_CLANG_TIDY ""
      C_INCLUDE_WHAT_YOU_USE ""
  )
  target_compile_options(${target_name} PRIVATE -w)
endfunction()

# Ignore a set of checks for first party target
function(ignore_gfx_target target_name)
  set(options WARNINGS CLANG_TIDY)
  cmake_parse_arguments(
    GFX_IGNORE
    "${options}"
    ""
    ""
    ${ARGN}
  )

  if(GFX_IGNORE_CLANG_TIDY)
    set_target_properties(
      ${target_name} PROPERTIES CXX_CLANG_TIDY "" C_CLANG_TIDY ""
    )
  endif()

  if(GFX_IGNORE_WARNINGS)
    target_compile_options(${target_name} PRIVATE -w)
  endif()

endfunction()
