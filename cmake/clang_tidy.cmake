set(GFX_CLANG_TIDY_EXECUTABLE
  clang-tidy;
)

find_library(GFX_MODULE_LIBRARY
  "libgfx_checks.so"
  PATH
    "/home/divak/repos/gfx-checks/build/lib"
)

set(GFX_CXX_CLANG_TIDY_OPTIONS
  -load=${GFX_MODULE_LIBRARY}
  --extra-arg=-stdlib=libc++;
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
)

set(GFX_CLANG_TIDY_CATCH2_OPTIONS
  --checks=-readability-function-size;
)

set(GFX_CLANG_TIDY_CATCH2_TARGET_PROPERTIES
  ${GFX_CLANG_TIDY_EXECUTABLE};
  ${GFX_CXX_CLANG_TIDY_OPTIONS};
  ${GFX_CLANG_TIDY_CATCH2_OPTIONS};
)

if (GFX_LINTING)
  message(STATUS "gfx::ENABLED clang-tidy")
  set(CMAKE_CXX_CLANG_TIDY
    ${GFX_CLANG_TIDY_EXECUTABLE};
    ${GFX_CXX_CLANG_TIDY_OPTIONS};
  )
  set(CMAKE_C_CLANG_TIDY
    ${GFX_CLANG_TIDY_EXECUTABLE};
    ${GFX_C_CLANG_TIDY_OPTIONS};
  )
else()
  message(STATUS "gfx::DISABLED clang-tidy")
  unset(CMAKE_CXX_CLANG_TIDY)
  unset(CMAKE_CXX_INCLUDE_WHAT_YOU_USE)
  set(GFX_CLANG_TIDY_CATCH2_TARGET_PROPERTIES "")
endif()
