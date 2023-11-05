set(GFX_CLANG_TIDY_EXECUTABLE clang-tidy;)

find_package(GFXTidy 0.0.1 REQUIRED)

get_property(
  GFX_TIDY_LOCATION
  TARGET GFXTidy::gfx_tidy_checks
  PROPERTY LOCATION
)

# Append check to a target, only supporting single check
function(append_clang_tidy_check)
  set(one_value_args TARGET CHECK)
  cmake_parse_arguments(
    APPEND_CHECK
    ""
    "${one_value_args}"
    ""
    ${ARGN}
  )

  if(${GFX_CLANG_TIDY})
    set_property(
      TARGET ${APPEND_CHECK_TARGET}
      APPEND
      PROPERTY CXX_CLANG_TIDY "--checks=${APPEND_CHECK_CHECK}"
    )
  endif()

endfunction()

set(GFX_CXX_CLANG_TIDY_OPTIONS
    -load=${GFX_TIDY_LOCATION}
    --extra-arg=-DGFX_CLANG_TIDY;
    --extra-arg=-Wno-unused-command-line-argument;
    --extra-arg=-Wno-error=unknown-warning-option;
    --extra-arg=-Weverything;
    --extra-arg=-Wno-c++98-compat;
    --extra-arg=-Wno-c++98-compat-pedantic;
    --extra-arg=-Wno-switch-enum;
    --extra-arg=-Wno-padded;
)

set(GFX_C_CLANG_TIDY_OPTIONS
    --extra-arg=-DGFX_CLANG_TIDY;
    --extra-arg=-Wno-unused-command-line-argument;
    --extra-arg=-Wno-error=unknown-warning-option;
    --extra-arg=-Wno-declaration-after-statement;
    --extra-arg=-Weverything;
    --extra-arg=-Wno-switch-enum;
    --extra-arg=-Wno-padded;
    --extra-arg=-Wno-unsafe-buffer-usage; # TODO C++ specific?
)

set(GFX_CLANG_TIDY_CATCH2_OPTIONS --checks=-readability-function-size;)

set(GFX_CLANG_TIDY_CATCH2_TARGET_PROPERTIES
    ${GFX_CLANG_TIDY_EXECUTABLE};
    ${GFX_CXX_CLANG_TIDY_OPTIONS};
    ${GFX_CLANG_TIDY_CATCH2_OPTIONS};
)

if(GFX_CLANG_TIDY)
  message(STATUS "gfx::ENABLED clang-tidy")
  set(CMAKE_CXX_CLANG_TIDY
      ${GFX_CLANG_TIDY_EXECUTABLE}; ${GFX_CXX_CLANG_TIDY_OPTIONS};
  )
  set(CMAKE_C_CLANG_TIDY
      ${GFX_CLANG_TIDY_EXECUTABLE}; ${GFX_C_CLANG_TIDY_OPTIONS};
  )
else()
  message(STATUS "gfx::DISABLED clang-tidy")
  unset(CMAKE_CXX_CLANG_TIDY)
  unset(CMAKE_C_CLANG_TIDY)
  set(GFX_CLANG_TIDY_CATCH2_TARGET_PROPERTIES "")
endif()
