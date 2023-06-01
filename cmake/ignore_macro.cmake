macro(ignore_third_party target_name)
  set_target_properties(
    ${target_name}
    PROPERTIES CXX_CLANG_TIDY ""
               CXX_INCLUDE_WHAT_YOU_USE ""
               C_CLANG_TIDY ""
               C_INCLUDE_WHAT_YOU_USE "")
  target_compile_options(${target_name} PRIVATE -w)
endmacro()

macro(ignore_gfx_target target_name)
  set(OptionalArgs ${ARGN})
  foreach(OptionalArg ${OptionalArgs})
    if("${OptionalArg}" STREQUAL "clang-tidy")
      set_target_properties(${target_name} PROPERTIES CXX_CLANG_TIDY ""
                                                      C_CLANG_TIDY "")
    elseif("${OptionalArg}" STREQUAL "warnings")
      target_compile_options(${target_name} PRIVATE -w)
    endif()
  endforeach()
endmacro()
