set(CMAKE_CXX_COMPILER g++)
set(CMAKE_C_COMPILER gcc)

string(APPEND GFX_GCC_FLAGS " -Werror" # " -O3"
)

string(
  APPEND
  GFX_GCC_WARNINGS
  " -Wall"
  " -Wextra"
  " -Wshadow"
  " -Wpedantic"
  " -Wunused"
  " -Wunused-macros"
  " -Wmissing-declarations"
  " -Wredundant-decls"
  " -Wcast-align"
  " -Wconversion"
  " -Wsign-conversion"
  " -Wint-in-bool-context"
  " -Warith-conversion"
  " -Wdouble-promotion"
  " -Wfloat-equal"
  " -Wmisleading-indentation"
  " -Wduplicated-cond"
  " -Wduplicated-branches"
  " -Wlogical-op"
  " -Wnull-dereference"
  " -Walloc-zero"
  " -Wwrite-strings"
  " -Wformat=2"
  " -Wundef"
  " -Wsuggest-attribute=cold"
  " -Wsuggest-attribute=malloc"
  " -Wsuggest-attribute=noreturn"
  " -Wsuggest-final-types"
  " -Wsuggest-final-methods"
  " -Wbidi-chars=any,ucn"
  " -Warray-compare"
  " -Wcast-qual"
  " -Wimplicit-fallthrough"
)

string(
  APPEND
  GFX_CXX_WARNING_FLAGS
  " -Wnon-virtual-dtor"
  " -Wold-style-cast"
  " -Woverloaded-virtual"
  " -Wuseless-cast"
  " -Weffc++"
  " -Wvexing-parse"
  " -Wsuggest-override"
  " -Wc++20-compat"
  " -Wterminate"
  " -Wsubobject-linkage"
  " -Wrange-loop-construct"
  " -Wpessimizing-move"
  " -Wextra-semi"
  " -Wconversion-null"
  " -Wredundant-move"
  " -Wredundant-tags"
  " -Wreorder"
  " -Wsign-promo"
  " -Wuninitialized"
)

string(
  APPEND
  GFX_C_WARNING_FLAGS
  " -Wno-declaration-after-statement" # C90 compatibility
  " -Wmissing-prototypes"
  " -Wc++-compat"
)

string(
  APPEND
  CMAKE_CXX_FLAGS
  ${GFX_GCC_FLAGS}
  ${GFX_GCC_WARNINGS}
  ${GFX_CXX_WARNING_FLAGS}
)

string(
  APPEND
  CMAKE_C_FLAGS
  ${GFX_GCC_FLAGS}
  ${GFX_GCC_WARNINGS}
  ${GFX_C_WARNING_FLAGS}
)

set(CUDA_HOST_COMPILER ${CMAKE_CXX_COMPILER})
set(CMAKE_CUDA_COMPILER /usr/local/cuda-12.1/bin/nvcc)

string(
  APPEND
  CMAKE_CUDA_FLAGS
  " --generate-line-info"
  " -std=c++20"
)

string(
  APPEND
  GFX_GCC_ASAN
  " -fsanitize=address"
  " -fsanitize=leak"
  " -fno-omit-frame-pointer"
  " -fsanitize-address-use-after-scope"
  " -fno-optimize-sibling-calls"
  " -fno-common"
)

string(ASCII 27 Esc)
set(RED "${Esc}[31m")
set(YELLOW "${Esc}[93m")
set(RESET "${Esc}[m")

if(GFX_ASAN)
  string(APPEND CMAKE_C_FLAGS ${GFX_GCC_ASAN})
  string(APPEND CMAKE_CXX_FLAGS ${GFX_GCC_ASAN})

  if(NOT DEFINED ENV{ASAN_OPTIONS})
    string(
      APPEND
      ASAN_ENV_MSG
      "${RESET}${YELLOW}"
      "gfx::Set ASAN environment variable for run-time flags:"
      "\n"
      "export ASAN_OPTIONS=protect_shadow_gap=0,fast_unwind_on_malloc=0"
      "${RESET}${RED}\n"
    )

    message(FATAL_ERROR "${ASAN_ENV_MSG}")
  endif()
endif()

# Call in project listing after configuring build to log in terminal
function(gfx_print_asan_info)
  if(GFX_ASAN)
    message(NOTICE "Disable unloading to not confuse sanitizers:")
    set(fake_dl_close "./libdlclose.so")
    message(
      NOTICE
      "${RESET}${YELLOW}clang --shared "
      "../tools/fake/dlclose.c"
      " -o libdlclose.so${RESET}"
    )
    message(
      NOTICE
      "${RESET}${YELLOW}LD_PRELOAD=${fake_dl_close} <executable>${RESET}"
    )
  endif()
endfunction()

# Call in project listing after configuring build to log in terminal
function(gfx_print_ubsan_info)
  if(GFX_UBSAN)
    message(
      NOTICE
      "${RESET}${YELLOW}"
      "UBSAN not implemented for GCC"
      "${RESET}"
    )
  endif()
endfunction()
