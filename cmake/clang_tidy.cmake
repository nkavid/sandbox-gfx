set(GFX_CLANG_TIDY_EXECUTABLE clang-tidy;)

find_package(GFXTidy 0.0.1 REQUIRED)

get_property(
  GFX_TIDY_LOCATION
  TARGET GFXTidy::gfx_tidy_checks
  PROPERTY LOCATION
)

set(GFX_CXX_CLANG_TIDY_OPTIONS
    -load=${GFX_TIDY_LOCATION}
    --extra-arg=-nostdinc++;
    --extra-arg=-Wno-error=unknown-warning-option;
    --extra-arg=-Weverything;
    --extra-arg=-Wno-c++98-compat;
    --extra-arg=-Wno-c++98-compat-pedantic;
    --extra-arg=-Wno-switch-enum;
    --extra-arg=-Wno-padded;
)

set(GFX_C_CLANG_TIDY_OPTIONS
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

if(GFX_LINTING)
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
